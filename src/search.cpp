#include "move_picker.h"
#include "logger.h"
#include "search.h"
#include "transposition_table.h"

#include <algorithm>
#include <chrono>
#include <cstring>

namespace engine
{

Score mate_in(int ply)
{
    return INFINITY_SCORE - 1;
}

bool is_score_mate(Score score)
{
    return score == INFINITY_SCORE - 1 || -score == INFINITY_SCORE - 1;
}

const int64_t INFINITE = 1LL << 32;

Search::Search(const Position& position, const PositionScorer& scorer, const Limits& limits)
    : position(position), scorer(scorer), limits(limits), pv_list(), nodes_searched(0), info(), _root_moves()
{
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
        int our_time = limits.timeleft[position.side_to_move()];
        int movestogo = limits.movestogo == 0 ? 20 : limits.movestogo;

        _search_time = our_time / (movestogo + 1);
        _search_depth = MAX_DEPTH;
    }
    else
    {
        _search_depth = 7;
        _search_time = INFINITE;
    }

}

void Search::stop()
{
    stop_search = true;
}

void Search::go()
{
    Position pos = position;
    stop_search = false;
    start_time = std::chrono::steady_clock::now();

    TimePoint end_time;
    int64_t elapsed = 0LL;

    MoveList temp_pv_list;

    Move* begin = MOVE_LIST[0];
    Move* end = generate_moves(position, position.side_to_move(), begin);

    for (Move* it = begin; it != end; ++it)
        _root_moves.push_back(std::make_pair(DRAW_SCORE, *it));

    int depth = 0;
    while (!stop_search)
    {
        depth++;
        nodes_searched = 0;

        std::sort(_root_moves.begin(), _root_moves.end(), std::greater<>());
        Score result = root_search(pos, depth, -INFINITY_SCORE, INFINITY_SCORE, temp_pv_list);

        end_time = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        if (!stop_search)
        {
            Position temp = position;
            pv_list = temp_pv_list;
            logger << "info "
                      << "depth " << depth << " "
                      << "score cp " << result / 2 << " "
                      << "nodes " << nodes_searched << " "
                      << "nps " << (nodes_searched * 1000 / (elapsed + 1)) << " "
                      << "time " << elapsed << " "
                      << "pv ";
            for (int i = pv_list.size() - 1; i >= 0; --i)
            {
                logger << temp.move_to_string(pv_list[i]) << " ";
                temp.do_move(pv_list[i]);
            }
            logger << std::endl;
        }

        if (is_score_mate(result))
            break;

        if (depth >= _search_depth)
            break;

        if (elapsed >= (_search_time / 2))
            break;
    }


    logger << "bestmove " << position.move_to_string(pv_list.back()) << std::endl;
}

Score Search::root_search(Position& position, int depth, Score alpha, Score beta, MoveList& movelist)
{
    movelist = MoveList();

    if (position.threefold_repetition())
        return DRAW_SCORE;
    if (position.rule50())
        return DRAW_SCORE;

    if (_root_moves.size() == 0)
    {
        if (position.is_in_check(position.side_to_move()))
            return -mate_in(depth);
        return DRAW_SCORE;
    }

    if (depth == 0)
        return quiescence_search(position, MAX_DEPTH - 1, alpha, beta);

    _best_move = _root_moves[0].second;

    Score bestscore = -INFINITY_SCORE;
    MoveList temp_movelist;

    for (int pos = 0; pos < _root_moves.size(); ++pos)
    {
        Move move = _root_moves[pos].second;
        MoveInfo moveinfo = position.do_move(move);

        info.ply++;
        Score result = -search(position, depth - 1, -beta, -alpha, temp_movelist);
        info.ply--;

        position.undo_move(move, moveinfo);

        if (stop_search || check_limits())
        {
            stop_search = true;
            return bestscore;
        }
        _root_moves[pos].first = result;

        if (result >= beta)
        {
            movelist = MoveList(temp_movelist);
            movelist.push_back(move);
            _best_move = move;
            return beta;
        }

        if (result > bestscore)
        {
            _best_move = move;
            bestscore = result;
            movelist = MoveList(temp_movelist);
            movelist.push_back(move);

        }
        if (result > alpha)
            alpha = result;
    }

    return bestscore;

}

Score Search::search(Position& position, int depth, Score alpha, Score beta, MoveList& movelist)
{
    movelist = MoveList();
    if (stop_search || check_limits())
    {
        stop_search = true;
        return 0;
    }

    if (position.threefold_repetition())
        return DRAW_SCORE;
    if (position.rule50())
        return DRAW_SCORE;

    if (depth == 0)
        return quiescence_search(position, MAX_DEPTH - 1, alpha, beta);

    Move* begin = MOVE_LIST[depth];
    Move* end = generate_moves(position, position.side_to_move(), begin);
    assert(end - begin >= 0);

    if (begin == end)
    {
        if (position.is_in_check(position.side_to_move()))
            return -mate_in(depth);
        return DRAW_SCORE;
    }

    Score best = -INFINITY_SCORE;
    MovePicker movepicker(position, begin, end, info, true);
    MoveList temp_movelist;

    while (movepicker.has_next())
    {
        Move move = movepicker.get_next();
        MoveInfo moveinfo = position.do_move(move);

        info.ply++;
        Score result = -search(position, depth - 1, -beta, -alpha, temp_movelist);
        info.ply--;

        position.undo_move(move, moveinfo);

        if (result >= beta)
        {
            if (position.piece_at(to(move)) == NO_PIECE)
            {
                info.update_killers(info.ply, move);
                info.update_history(position.side_to_move(), from(move), to(move), depth);
            }
            movelist = MoveList(temp_movelist);
            movelist.push_back(move);
            return beta;
        }

        if (result > best)
        {
            best = result;
            movelist = MoveList(temp_movelist);
            movelist.push_back(move);

        }
        if (result > alpha)
            alpha = result;
    }

    return best;
}

Score Search::quiescence_search(Position& position, int depth, Score alpha, Score beta)
{
    if (stop_search || check_limits())
    {
        stop_search = true;
        return 0;
    }

    if (position.threefold_repetition())
        return DRAW_SCORE;
    if (position.rule50())
        return DRAW_SCORE;

    bool is_in_check = position.is_in_check(position.side_to_move());

    Move* begin = QUIESCENCE_MOVE_LIST[depth];
    Move* end = generate_moves(position, position.side_to_move(), begin);

    if (begin == end)
        return is_in_check ? -mate_in(1) : DRAW_SCORE;

    end = generate_quiescence_moves(position, position.side_to_move(), begin);

    Score standpat = scorer.score(position);
    nodes_searched++;

    if (depth <= 0)
        return standpat;

    if (standpat >= beta)
        return beta;
    if (standpat > alpha)
        alpha = standpat;


    MovePicker movepicker(position, begin, end, info, false);

    while (movepicker.has_next())
    {
        Move move = movepicker.get_next();
        MoveInfo moveinfo = position.do_move(move);
        Score result = -quiescence_search(position, depth - 1, -beta, -alpha);
        position.undo_move(move, moveinfo);

        alpha = std::max(alpha, result);
        if (alpha >= beta)
            break;
    }

    return alpha;
}

bool Search::check_limits()
{
    check_limits_counter--;
    if (check_limits_counter > 0)
        return false;

    check_limits_counter = 4096;

    if (limits.nodes > 0 && nodes_searched >= limits.nodes)
    {
        stop_search = true;
        return true;
    }

    TimePoint end_time = std::chrono::steady_clock::now();
    int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    if (elapsed >= _search_time)
    {
        stop_search = true;
        return true;
    }

    return false;
}

}
