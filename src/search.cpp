#include "endgame.h"
#include "move_picker.h"
#include "logger.h"
#include "search.h"
#include "time_manager.h"
#include "transposition_table.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <cmath>

namespace engine
{

Move PV_LIST[2 * MAX_DEPTH][2 * MAX_DEPTH];
int PV_LIST_LENGTH[2 * MAX_DEPTH];

void set_new_pv(int depth, int move)
{
    assert(depth < 2 * MAX_DEPTH - 1);
    PV_LIST_LENGTH[depth] = PV_LIST_LENGTH[depth + 1] + 1;
    for (int i = 0; i < PV_LIST_LENGTH[depth + 1]; ++i)
        PV_LIST[depth][i + 1] = PV_LIST[depth + 1][i];
    PV_LIST[depth][0] = move;
}

std::string score2str(Value score)
{
    if (score <= lost_in(MAX_DEPTH))
        return "mate -" + std::to_string(score + INFINITY_SCORE);
    else if (score >= win_in(MAX_DEPTH))
        return "mate " + std::to_string(INFINITY_SCORE - score);
    else
        return "cp " + std::to_string(score * 100LL / 300LL);
}

const bool is_mate(Value score)
{
    return score < lost_in(MAX_DEPTH) || score > win_in(MAX_DEPTH);
}

const Duration INFINITE = 1LL << 32;

Search::Search(const Position& position, const Limits& limits)
    : _position(position)
    , _scorer()
    , limits(limits)
    , check_limits_counter(4096)
    , stop_search(false)
    , _search_time(0)
    , _search_depth(0)
    , _max_nodes_searched(limits.nodes)
    , _current_depth(0)
    , _best_move(NO_MOVE)
    , _start_time()
    , _nodes_searched(0)
    , _root_moves()
    , _ply_counter(0)
    , _ttable()
    , _info(_ttable)
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
        Move* end = generate_moves(_position, _position.side_to_move(), begin);
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
    else if (limits.timeleft[position.side_to_move()] != 0)
    {
        _search_time = TimeManager::calculateTime(limits, position.side_to_move(), position.ply_count());
        _search_depth = MAX_DEPTH;
    }
    else
    {
        _search_depth = 7;
        _search_time = INFINITE;
    }

    if (_max_nodes_searched == 0)
        _max_nodes_searched = INFINITE;
}

void Search::stop()
{
    stop_search = true;
}

void Search::go()
{
    _ttable.clear();
    _scorer.clear();
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
    logger << "bestmove " << _position.move_to_string(_best_move) << std::endl;
    _ttable.clear();
}

MoveList Search::get_pv(int length)
{
    Position curr_position = _position;
    MoveList pv;
    int curr_length = 0;

    const tt::TTEntry* entryPtr = _ttable.get(curr_position.hash());

    Move* begin = MOVE_LIST[0];
    Move* end = generate_moves(curr_position, curr_position.side_to_move(), begin);

    while (curr_length < length && entryPtr && std::find(begin, end, entryPtr->move) != end)
    {
        pv.push_back(entryPtr->move);
        curr_position.do_move(entryPtr->move);
        curr_length++;
        entryPtr = _ttable.get(curr_position.hash());
        end = generate_moves(curr_position, curr_position.side_to_move(), begin);
    }

    return pv;
}

void Search::print_info(Value result, int depth, int64_t nodes_searched, int64_t elapsed)
{
    logger << "info "
           << "depth " << depth << " "
           << "score " << score2str(result) << " "
           << "nodes " << nodes_searched << " "
           << "nps " << (nodes_searched * 1000 / (elapsed + 1)) << " "
           << "time " << elapsed << " "
           << "pv ";
    Position temp_position = _position;
    for (int i = 0; i < PV_LIST_LENGTH[0]; ++i)
    {
        Move m = PV_LIST[0][i];
        logger << temp_position.move_to_string(m) << " ";
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
    Value min_bound = -INFINITY_SCORE;
    Value max_bound = INFINITY_SCORE;

    _current_depth = 0;
    while (!stop_search)
    {
        _current_depth++;
        _nodes_searched = 0;
        _ply_counter = 0;

        Value result;
        Value delta = 100;

        if (_current_depth >= 4)
        {
            min_bound = std::max(previous_score - delta, -INFINITY_SCORE);
            max_bound = std::min(previous_score + delta, INFINITY_SCORE);
        }

        while (true)
        {
            assert(_ply_counter == 0);
            assert(min_bound < max_bound);
            result = root_search(_position, _current_depth, min_bound, max_bound);

            if (result <= min_bound)
                min_bound = std::max(min_bound - delta, -INFINITY_SCORE);
            else if (result >= max_bound)
                max_bound = std::min(max_bound + delta, INFINITY_SCORE);
            else
                break;

            if (check_limits())
                break;

            delta = static_cast<Value>(static_cast<float>(delta) * 1.25f);
        }

        previous_score = result;

        end_time = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - _start_time).count();

        if (!stop_search)
        {
            assert(PV_LIST_LENGTH[0] > 0);
            print_info(result, _current_depth, _nodes_searched, elapsed);
            _best_move = PV_LIST[0][0];
        }

        if (result < lost_in(MAX_DEPTH) || result > win_in(MAX_DEPTH))
            break;

        if (_current_depth >= _search_depth)
            break;

        if (elapsed >= (_search_time / 2))
            break;
    }

}

Value Search::root_search(Position& position, int depth, Value alpha, Value beta)
{
    assert(depth > 0);
    assert(alpha <= beta);
    bool is_in_check = position.is_in_check(position.side_to_move());

    PV_LIST_LENGTH[_ply_counter] = 0;

    // if there are no legal moves
    // return draw if we are not in check (stalemate)
    // otherwise lost (checkmate)
    if (_root_moves.size() == 0)
        return is_in_check ? lost_in(_ply_counter) : DRAW_SCORE;

    // check extension
    if (is_in_check)
        depth++;

    Move* begin = _root_moves.data();
    Move* end = _root_moves.data() + _root_moves.size();

    MovePicker movepicker(position, begin, end, _info, true);
    Move curr_best_move = NO_MOVE;

    bool search_full_window = true;
    while (movepicker.has_next())
    {
        Move move = movepicker.get_next();
        MoveInfo moveinfo = position.do_move(move);
        _nodes_searched++;

        _info.ply++;
        _ply_counter++;
        Value result;
        if (search_full_window)
        {
            result = -search<true>(position, depth - 1, -beta, -alpha);
        }
        else
        {
            result = -search<true>(position, depth - 1, -alpha - 1, -alpha);
            if (alpha < result && result < beta)
                result = -search<true>(position, depth - 1, -beta, -alpha);
        }
        _info.ply--;
        _ply_counter--;


        position.undo_move(move, moveinfo);

        if (stop_search || check_limits())
        {
            stop_search = true;
            return alpha;
        }

        if (result > alpha)
        {
            search_full_window = false;
            alpha = result;
            curr_best_move = move;
            set_new_pv(_ply_counter, move);

            // mate found
            if (result > win_in(MAX_DEPTH))
            {
                break;
            }

        }
    }

    // if no move have been found just select the first one
    if (curr_best_move == NO_MOVE)
    {
        curr_best_move = _root_moves[0];
        PV_LIST_LENGTH[_ply_counter] = 1;
        PV_LIST[_ply_counter][0] = curr_best_move;
    }


    if (!stop_search)
    {
        tt::TTEntry entry(alpha, depth, tt::Flag::kEXACT, curr_best_move);
        _ttable.update(position.hash(), entry);
    }
    return alpha;
}

template <bool allow_null_move>
Value Search::search(Position& position, int depth, Value alpha, Value beta)
{
    assert(alpha < beta);
    PV_LIST_LENGTH[_ply_counter] = 0;

    if (stop_search || check_limits())
    {
        stop_search = true;
        return Value(0);
    }

    if (position.is_draw())
        return DRAW_SCORE;

    /* std::vector<Move> moves(MAX_MOVES, NO_MOVE); */
    /* Move* begin = moves.data(); */
    Move* begin = MOVE_LIST[_ply_counter];
    Move* end = generate_moves(position, position.side_to_move(), begin);

    Value alphaOriginal = alpha;
    const tt::TTEntry* entryPtr = _ttable.get(position.hash());
    if (entryPtr && (entryPtr->depth >= depth) && (std::find(begin, end, entryPtr->move) != end))
    {
        switch (entryPtr->flag)
        {
            case tt::Flag::kEXACT:
                PV_LIST_LENGTH[_ply_counter] = 1;
                PV_LIST[_ply_counter][0] = entryPtr->move;
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
            return Value(entryPtr->score);
        }
    }

    bool is_in_check = position.is_in_check(position.side_to_move());

    if (begin == end)
        return is_in_check ? lost_in(_ply_counter) : DRAW_SCORE;

    if (is_in_check)
        depth++;

    if (depth == 0)
        return quiescence_search(position, MAX_DEPTH - 1, alpha, beta);

    if constexpr (allow_null_move)
    {
        Color side = position.side_to_move();
        int num_of_pieces = position.number_of_pieces(make_piece(side, KNIGHT))
              + position.number_of_pieces(make_piece(side, BISHOP))
              + position.number_of_pieces(make_piece(side, ROOK))
              + position.number_of_pieces(make_piece(side, QUEEN));
        if (!is_in_check && num_of_pieces > 0 && depth > 4)
        {
            _ply_counter++;
            _info.ply++;
            MoveInfo moveinfo = position.do_null_move();
            Value result = -search<false>(position, depth - 4, -beta, -alpha);
            position.undo_null_move(moveinfo);
            _info.ply--;
            _ply_counter--;

            if (result >= beta)
            {
                return beta;
            }
        }
    }

    Move best_move = NO_MOVE;
    MovePicker movepicker(position, begin, end, _info, true);


    bool search_full_window = true;
    while (movepicker.has_next())
    {
        Move move = movepicker.get_next();
        MoveInfo moveinfo = position.do_move(move);
        _nodes_searched++;

        _info.ply++;
        _ply_counter++;
        Value result;
        if (search_full_window)
        {
            result = -search<true>(position, depth - 1, -beta, -alpha);
        }
        else
        {
            result = -search<true>(position, depth - 1, -alpha - 1, -alpha);
            if (alpha < result && result < beta)
                result = -search<true>(position, depth - 1, -beta, -alpha);
        }
        _info.ply--;
        _ply_counter--;

        position.undo_move(move, moveinfo);

        if (result >= beta)
        {
            if (position.piece_at(to(move)) == NO_PIECE)
            {
                _info.update_killers(_info.ply, move);
                _info.update_history(position.side_to_move(), from(move), to(move), _info.ply);
            }

            assert(move != NO_MOVE);
            tt::TTEntry entry(result, depth, tt::Flag::kLOWER_BOUND, move);
            _ttable.update(position.hash(), entry);

            set_new_pv(_ply_counter, move);

            return beta;
        }

        if (result > alpha)
        {
            alpha = result;
            search_full_window = false;
            best_move = move;
            set_new_pv(_ply_counter, move);
        }

    }

    if (best_move == NO_MOVE)
    {
        best_move = begin[0];
        PV_LIST_LENGTH[_ply_counter] = 1;
        PV_LIST[_ply_counter][0] = best_move;
    }

    tt::Flag flag = alpha <= alphaOriginal
                  ? tt::Flag::kUPPER_BOUND
                  : tt::Flag::kEXACT;
    tt::TTEntry entry(alpha, depth, flag, best_move);
    _ttable.update(position.hash(), entry);

    return alpha;
}

Value Search::quiescence_search(Position& position, int depth, Value alpha, Value beta)
{
    if (stop_search || check_limits())
    {
        stop_search = true;
        return 0;
    }

    if (position.is_draw())
        return DRAW_SCORE;

    bool is_in_check = position.is_in_check(position.side_to_move());

    /* std::vector<Move> moves(MAX_MOVES, NO_MOVE); */
    /* Move* begin = moves.data(); */
    Move* begin = QUIESCENCE_MOVE_LIST[depth];
    Move* end = generate_moves(position, position.side_to_move(), begin);

    if (begin == end)
        return is_in_check ? lost_in(_ply_counter) : DRAW_SCORE;

    end = generate_quiescence_moves(position, position.side_to_move(), begin);

    Value standpat = _scorer.score(position);

    if (depth <= 0)
        return standpat;

    if (standpat >= beta)
        return beta;
    if (standpat > alpha)
        alpha = standpat;


    MovePicker movepicker(position, begin, end, _info, false);

    bool search_full_window = true;
    while (movepicker.has_next())
    {
        Move move = movepicker.get_next();
        MoveInfo moveinfo = position.do_move(move);
        _nodes_searched++;

        Value result;
        _ply_counter++;
        if (search_full_window)
            result = -quiescence_search(position, depth - 1, -beta, -alpha);
        else
        {
            result = -quiescence_search(position, depth - 1, -alpha - 1, -alpha);
            if (alpha < result && result < beta)
                result = -quiescence_search(position, depth - 1, -beta, -alpha);
        }
        position.undo_move(move, moveinfo);
        _ply_counter--;

        if (result >= beta)
            return beta;
        if (result > alpha)
        {
            alpha = result;
            search_full_window = false;
        }
    }

    return alpha;
}

bool Search::check_limits()
{
    check_limits_counter--;
    if (check_limits_counter > 0)
        return false;

    check_limits_counter = 40960;

    if (_nodes_searched >= _max_nodes_searched)
    {
        stop_search = true;
        return true;
    }

    TimePoint end_time = std::chrono::steady_clock::now();
    int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - _start_time).count();

    if (elapsed >= _search_time)
    {
        stop_search = true;
        return true;
    }

    return false;
}

}
