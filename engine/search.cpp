#include "search.h"

#include "endgame.h"
#include "logger.h"
#include "search_utils.h"
#include "time_manager.h"
#include "transposition_table.h"
#include "utils.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>

namespace engine
{

int late_move_reduction(int depth, int move_number)
{
    move_number = std::min(move_number, 64);
    return static_cast<int>(std::floor(1 + std::log(depth) * std::log(move_number)));
}

std::string score2str(Value score)
{
    if (score <= lost_in(MAX_DEPTH))
        return "mate -" + std::to_string(VALUE_MATE + score);
    else if (score >= win_in(MAX_DEPTH))
        return "mate " + std::to_string(VALUE_MATE - score);
    else
        return "cp " + std::to_string(score * 100LL / 300LL);
}

void clear_pv_list(Info* info)
{
    assert(info != nullptr);
    info->_pv_list_length = 0;
}

void set_new_pv_list(Info* info, Move move)
{
    assert(info != nullptr);
    info->_pv_list_length = 1;
    info->_pv_list[0] = move;
}

void add_new_move_to_pv_list(Info* destInfo, Move move, Info* srcInfo)
{
    assert(srcInfo != nullptr);
    assert(destInfo != nullptr);
    const int len = srcInfo->_pv_list_length;
    assert(len < MAX_DEPTH * 2);
    destInfo->_pv_list_length = len + 1;
    destInfo->_pv_list[0] = move;
    std::memcpy(destInfo->_pv_list.data() + 1, srcInfo->_pv_list.data(),
                len * sizeof(Move));
}

void add_bonus(int* v, int b)
{
    int x = *v;
    *v = std::min(x + b, 20000);
}

void update_move_scores(const Position& position, Move move, Info* info,
                        HistoryScore& historyScore, int depth)
{
    Square from_sq = from(move);
    Square to_sq = to(move);
    Piece moved_piece = position.piece_at(from_sq);

    if (move != info->_killer_moves[0])
    {
        info->_killer_moves[1] = info->_killer_moves[0];
        info->_killer_moves[0] = move;
    }

    int bonus = depth * depth;

    add_bonus(&historyScore[position.color()][from_sq][to_sq], bonus);
    if ((info-1)->_current_move != NO_MOVE)
        add_bonus(&(*(info-1)->_counter_move)[moved_piece][to_sq], bonus);
}

const Duration INFINITE = 1LL << 32;

Search::Search(const Position& position, const Limits& limits)
    : _position(position),
      _scorer(),
      limits(limits),
      check_limits_counter(4096),
      stop_search(false),
      _search_time(0),
      _search_depth(0),
      _max_nodes_searched(limits.nodes),
      _current_depth(0),
      _best_move(NO_MOVE),
      _start_time(),
      _nodes_searched(0),
      _root_moves(),
      _ttable(),
      _stack_info(),
      _history_score(),
      _move_orderer(_ttable, _history_score),
      _counter_move_table()
{
    if (limits.searchmovesnum > 0)
    {
        const Move* begin = limits.searchmoves;
        const Move* end = limits.searchmoves + limits.searchmovesnum;
        _root_moves.insert(_root_moves.end(), begin, end);
    }
    else
    {
        Move* begin = MOVE_LIST[0];
        Move* end = generate_moves(_position, _position.color(), begin);
        _root_moves.insert(_root_moves.end(), begin, end);
    }

    if (limits.infinite)
    {
        _search_depth = MAX_DEPTH;
        _search_time = INFINITE;
    }
    else if (limits.depth != 0)
    {
        _search_depth = limits.depth;
        _search_time = INFINITE;
    }
    else if (limits.movetime != 0)
    {
        _search_depth = MAX_DEPTH;
        _search_time = limits.movetime;
    }
    else if (limits.timeleft[position.color()] != 0)
    {
        _search_time = TimeManager::calculateTime(limits, position.color(),
                                                  position.ply_count());
        _search_depth = MAX_DEPTH;
    }
    else
    {
        _search_depth = 7;
        _search_time = INFINITE;
    }

    if (_max_nodes_searched == 0) _max_nodes_searched = INFINITE;
}

void Search::stop()
{
    stop_search = true;
}

void Search::go()
{
    _ttable.clear();
    _scorer.clear();
    init_search();
    stop_search = false;
    _start_time = std::chrono::steady_clock::now();

    // check if there is only one move to make
    if (_root_moves.size() == 1)
    {
        _best_move = _root_moves[0];
    }
    else
    {
        iter_search();
    }

    assert(_best_move != NO_MOVE);
    logger << "bestmove " << _position.uci(_best_move) << std::endl;
}

void Search::init_search()
{
    for (Color c : {WHITE, BLACK})
        for (Square sq1 = SQ_A1; sq1 <= SQ_H8; ++sq1)
            for (Square sq2 = SQ_A1; sq2 <= SQ_H8; ++sq2)
                _history_score[c][sq1][sq2] = 0;

    for (Piece p1 = W_PAWN; p1 <= B_KING; ++p1)
    {
        for (Square sq1 = SQ_A1; sq1 <= SQ_H8; ++sq1)
        {
            for (Piece p2 = W_PAWN; p2 <= B_KING; ++p2)
                for (Square sq2 = SQ_A1; sq2 <= SQ_H8; ++sq2)
                    _counter_move_table[p1][sq1][p2][sq2] = 0;
        }
    }
}

void Search::print_info(Value result, int depth, int64_t nodes_searched,
                        int64_t elapsed, Info* info)
{
    logger << "info "
           << "depth " << depth << " "
           << "score " << score2str(result) << " "
           << "nodes " << nodes_searched << " "
           << "nps " << (nodes_searched * 1000 / (elapsed + 1)) << " "
           << "time " << elapsed << " "
           << "pv ";
    Position temp_position = _position;
    for (int i = 0; i < info->_pv_list_length; ++i)
    {
        Move m = info->_pv_list[i];
        logger << temp_position.uci(m) << " ";
        temp_position.do_move(m);
    }
    logger << std::endl;
}

void Search::iter_search()
{
    _best_move = NO_MOVE;

    TimePoint end_time;
    int64_t elapsed = 0LL;

    MoveList pv_list;
    Value previous_score;
    Value min_bound = -VALUE_INFINITE;
    Value max_bound = VALUE_INFINITE;

    Info* info = _stack_info.data();
    info->_ply = -1;
    info->_current_move = NO_MOVE;
    info->_counter_move = &_counter_move_table[NO_PIECE][1];

    _current_depth = 0;
    while (!stop_search)
    {
        _current_depth++;
        _nodes_searched = 0;

        Value result;
        Value delta = 100;

        if (_current_depth >= 4)
        {
            min_bound = std::max(previous_score - delta, -VALUE_INFINITE);
            max_bound = std::min(previous_score + delta, VALUE_INFINITE);
        }

        while (true)
        {
            assert(min_bound < max_bound);
            result = search(_position, _current_depth, min_bound, max_bound,
                            info + 1);

            if (result <= min_bound)
                min_bound = std::max(min_bound - delta, -VALUE_INFINITE);
            else if (result >= max_bound)
                max_bound = std::min(max_bound + delta, VALUE_INFINITE);
            else
                break;

            if (stop_search || check_limits()) break;

            delta =
                std::min(static_cast<Value>(static_cast<float>(delta) * 1.25f),
                         VALUE_INFINITE);
        }

        previous_score = result;

        end_time = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                      end_time - _start_time)
                      .count();

        if (!stop_search)
        {
            Info* realInfo = info + 1;
            assert(realInfo->_pv_list_length > 0);
            print_info(result, _current_depth, _nodes_searched, elapsed,
                       realInfo);
            _best_move = realInfo->_pv_list[0];
        }

        if (result < lost_in(MAX_DEPTH) || result > win_in(MAX_DEPTH)) break;

        if (_current_depth >= _search_depth) break;

        if (elapsed >= 3 * (_search_time / 4)) break;
    }
}

Value Search::search(Position& position, int depth, Value alpha, Value beta,
                     Info* info)
{
    assert(alpha < beta);

    info->_ply = (info - 1)->_ply + 1;
    clear_pv_list(info);

    LOG_DEBUG("[%d] enter search depth=%d alpha=%ld beta=%ld", info->_ply,
              depth, alpha, beta);

    const bool ROOT_NODE = info->_ply == 0;
    const bool PV_NODE = beta != alpha + 1;
    const bool IS_NULL = (info - 1)->_current_move == NO_MOVE;

    if (stop_search || check_limits())
    {
        stop_search = true;
        return Value(0);
    }

    // cannot check it in ROOT_NODE as it might return
    // without any move
    if (!ROOT_NODE && position.is_repeated())
        RETURN_DEBUG(VALUE_DRAW);

    if (position.is_draw()) RETURN_DEBUG(VALUE_DRAW);

    Move* begin = MOVE_LIST[info->_ply];
    Move* end = generate_moves(position, position.color(), begin);
    const int n_moves = end - begin;

    bool is_in_check = position.is_in_check(position.color());

    if (n_moves == 0)
        RETURN_DEBUG(is_in_check ? lost_in((info - 1)->_ply) : VALUE_DRAW);

    Value alphaOriginal = alpha;
    const tt::TTEntry* entryPtr = _ttable.get(position.hash());
    if (entryPtr && (entryPtr->depth >= depth) &&
        (std::find(begin, end, entryPtr->move) != end))
    {
        switch (entryPtr->flag)
        {
        case tt::Flag::kEXACT:
            set_new_pv_list(info, entryPtr->move);
            return Value(entryPtr->score);
        case tt::Flag::kLOWER_BOUND:
            alpha = std::max(alpha, entryPtr->score);
            break;
        case tt::Flag::kUPPER_BOUND:
            beta = std::min(beta, entryPtr->score);
            break;
        }

        if (alpha >= beta)
        {
            RETURN_DEBUG(Value(entryPtr->score));
        }
    }

    if (is_in_check) depth++;

    if (depth == 0)
    {
        LOG_DEBUG("start quiescence_search");
        RETURN_DEBUG(
            quiescence_search(position, MAX_DEPTH - 1, alpha, beta, info));
    }

    if (is_in_check)
    {
        info->_static_eval = VALUE_NONE;
    }
    else
    {
        info->_static_eval = _scorer.score(position);
    }

    const bool improving = !is_in_check && info->_ply >= 2 && (info->_static_eval > (info - 2)->_static_eval || (info - 2)->_static_eval == VALUE_NONE);

    // null move pruning
    if (!IS_NULL && !is_in_check &&
        position.no_nonpawns(position.color()) > 0 && depth > 4 &&
        info->_static_eval >= beta)
    {
        int reducedDepth = 3 + depth / 4 - (info->_static_eval - beta) / 300;
        reducedDepth = std::max(reducedDepth, 0);
        LOG_DEBUG("nullmove reducion %d vs %d", reducedDepth, depth - 4);
        //int reducedDepth = depth - 4;

        LOG_DEBUG("do nullmove");
        MoveInfo moveinfo = position.do_null_move();
        info->_current_move = NO_MOVE;  // this means that this was a null move
        info->_counter_move = &_counter_move_table[NO_PIECE][0]; // trash
        Value result = -search(position, reducedDepth, -beta, -beta + 1, info + 1);
        LOG_DEBUG("undo nullmove");
        position.undo_null_move(moveinfo);

        if (result >= beta) RETURN_DEBUG(beta);
    }

    Move best_move = NO_MOVE;
    _move_orderer.order_moves(position, begin, end, info);

    bool search_full_window = true;
    for (int move_count = 0; move_count < n_moves; ++move_count)
    {
        Move move = begin[move_count];
        LOG_DEBUG("do move %s", position.uci(move).c_str());
        info->_current_move = move;
        info->_counter_move = &_counter_move_table[position.piece_at(from(move))][to(move)];
        MoveInfo moveinfo = position.do_move(move);
        _nodes_searched++;

        PieceKind capturedPiece = captured_piece(moveinfo);
        PieceKind promotedPiece = promotion(move);

        bool isQuiet = capturedPiece == NO_PIECE_KIND && promotedPiece == NO_PIECE_KIND;

        Value result;
        if (search_full_window)
        {
            result = -search(position, depth - 1, -beta, -alpha, info + 1);
        }
        else
        {
            int reduction = 0;
            Value updated_score = info->_static_eval + BASIC_PIECE_VALUE[capturedPiece] + BASIC_PIECE_VALUE[promotedPiece];
            if (depth > 3 && (isQuiet || updated_score <= alpha))
            {
                reduction = late_move_reduction(depth, move_count + 1);
                reduction -= 2 * static_cast<int>(PV_NODE);

                if (isQuiet)
                {
                    if (move == info->_killer_moves[0] || move == info->_killer_moves[1])
                        reduction--;
                }
                else
                {
                    reduction += std::min(2, static_cast<int>((alpha - updated_score) / 300));
                }
                reduction = std::min(depth - 1, std::max(reduction, 0));
            }

            result = -search(position, depth - 1 - reduction, -alpha - 1, -alpha, info + 1);

            if (reduction > 0 && result > alpha)
                result = -search(position, depth - 1, -alpha - 1, -alpha, info + 1);
            if (PV_NODE && alpha < result && result < beta)
                result = -search(position, depth - 1, -beta, -alpha, info + 1);
        }

        LOG_DEBUG("undo move %s", position.uci(move).c_str());
        position.undo_move(move, moveinfo);

        if (result >= beta)
        {
            assert(move != NO_MOVE);
            if (isQuiet)
            {
                update_move_scores(position, move, info, _history_score, depth);
            }

            tt::TTEntry entry(result, depth, tt::Flag::kLOWER_BOUND, move);
            _ttable.update(position.hash(), entry);

            add_new_move_to_pv_list(info, move, info + 1);

            RETURN_DEBUG(beta);
        }

        if (result > alpha)
        {
            alpha = result;
            search_full_window = false;
            best_move = move;
            add_new_move_to_pv_list(info, move, info + 1);
        }
    }

    if (best_move == NO_MOVE)
    {
        best_move = begin[0];
        set_new_pv_list(info, best_move);
    }

    tt::Flag flag =
        alpha <= alphaOriginal ? tt::Flag::kUPPER_BOUND : tt::Flag::kEXACT;
    tt::TTEntry entry(alpha, depth, flag, best_move);
    _ttable.update(position.hash(), entry);

    RETURN_DEBUG(alpha);
}

Value Search::quiescence_search(Position& position, int depth, Value alpha,
                                Value beta, Info* info)
{
    info->_ply = (info - 1)->_ply + 1;
    clear_pv_list(info);

    LOG_DEBUG("[%d] enter quiescence_search depth=%d alpha=%ld beta=%ld",
              info->_ply, depth, alpha, beta);

    if (stop_search || check_limits())
    {
        stop_search = true;
        return 0;
    }

    if (position.is_draw()) RETURN_DEBUG(VALUE_DRAW);

    bool is_in_check = position.is_in_check(position.color());

    Move* begin = MOVE_LIST[info->_ply];
    Move* end = generate_moves(position, position.color(), begin);
    const int n_moves = end - begin;

    if (n_moves == 0)
        RETURN_DEBUG(is_in_check ? lost_in((info - 1)->_ply) : VALUE_DRAW);

    Value standpat = _scorer.score(position);

    if (depth <= 0) RETURN_DEBUG(standpat);

    if (standpat >= beta) RETURN_DEBUG(beta);
    if (standpat > alpha) alpha = standpat;

    _move_orderer.order_moves(position, begin, end, info);

    bool search_full_window = true;
    for (int move_count = 0; move_count < n_moves; ++move_count)
    {
        Move move = begin[move_count];
        info->_current_move = move;
        info->_counter_move = &_counter_move_table[position.piece_at(from(move))][to(move)];
        if (!is_in_check && position.piece_at(to(move)) == NO_PIECE &&
            promotion(move) == NO_PIECE_KIND)
            continue;
        LOG_DEBUG("do move %s", position.uci(move).c_str());
        MoveInfo moveinfo = position.do_move(move);
        _nodes_searched++;

        Value result;
        if (search_full_window)
            result = -quiescence_search(position, depth - 1, -beta, -alpha,
                                        info + 1);
        else
        {
            result = -quiescence_search(position, depth - 1, -alpha - 1, -alpha,
                                        info + 1);
            if (alpha < result && result < beta)
                result = -quiescence_search(position, depth - 1, -beta, -alpha,
                                            info + 1);
        }
        LOG_DEBUG("undo move %s", position.uci(move).c_str());
        position.undo_move(move, moveinfo);

        if (result >= beta) RETURN_DEBUG(beta);
        if (result > alpha)
        {
            alpha = result;
            search_full_window = false;
        }
    }

    RETURN_DEBUG(alpha);
}

bool Search::check_limits()
{
    check_limits_counter--;
    if (check_limits_counter > 0) return false;

    check_limits_counter = 40960;

    if (_nodes_searched >= _max_nodes_searched)
    {
        stop_search = true;
        return true;
    }

    TimePoint end_time = std::chrono::steady_clock::now();
    int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                          end_time - _start_time)
                          .count();

    if (elapsed >= _search_time)
    {
        stop_search = true;
        return true;
    }

    return false;
}

}  // namespace engine
