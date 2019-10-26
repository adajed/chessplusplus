#include "move_picker.h"
#include "search.h"
#include "transposition_table.h"

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

Search::Search(const Position& position, const PositionScorer& scorer, const Limits& limits)
    : position(position), scorer(scorer), limits(limits), thinking_time(0), nodes_searched(0)
{
    thinking_time = 0;
    if (limits.movetime > 0)
        thinking_time = limits.movetime;
    else if (!limits.infinite)
    {
        thinking_time = limits.timeleft[position.side_to_move()];
        if (thinking_time > 5000LL)
            thinking_time = 5000LL;
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
    nodes_searched = 0;

    int64_t result;
    if (limits.depth > 0)
    {
        result = search(pos, limits.depth, -INFINITY_SCORE, INFINITY_SCORE, pv_moves);
    }
    else
    {
        TimePoint end_time;
        int64_t elapsed = 0LL;
        bool found_some_move = false;

        Move *my_pv = new Move[MAX_DEPTH];

        int depth = 0;
        while (elapsed < (thinking_time / 2) && !stop_search && depth + 1 < MAX_DEPTH)
        {
            depth++;
            Score result = search(pos, depth, -INFINITY_SCORE, INFINITY_SCORE, my_pv);

            if (!stop_search)
            {
                found_some_move = true;
                std::memcpy(pv_moves, my_pv, depth * sizeof(Move));
            }

            end_time = std::chrono::steady_clock::now();
            elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

            if (is_score_mate(result))
                break;
        }

        assert(found_some_move);

        delete [] my_pv;
    }

    std::cout << "bestmove " << move_to_string(pv_moves[0])
              << " ponder " << move_to_string(pv_moves[1]) << std::endl;
}

Score Search::search(Position& position, int depth, Score alpha, Score beta, Move* pv_moves)
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
    Move* my_pv = new Move[depth - 1];

    MovePicker movepicker(position, begin, end);

    while (movepicker.has_next())
    {
        Move move = movepicker.get_next();
        MoveInfo moveinfo = position.do_move(move);

        Score result = -search(position, depth - 1, -beta, -alpha, my_pv);

        position.undo_move(move, moveinfo);

        if (result > best)
        {
            best = result;

            std::memcpy(pv_moves + 1, my_pv, (depth - 1) * sizeof(Move));
            pv_moves[0] = move;
        }
        if (result > alpha)
        {
            alpha = result;
            if (alpha >= beta)
                break;
        }
    }

    delete [] my_pv;

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

    if (depth <= 0)
        return scorer.score(position);

    Move* begin = QUIESCENCE_MOVE_LIST[depth];
    Move* end = generate_moves(position, position.side_to_move(), begin);
    assert(end - begin >= 0);

    bool is_in_check = position.is_in_check(position.side_to_move());

    if (begin == end)
    {
        if (is_in_check)
            return -mate_in(1);
        return DRAW_SCORE;
    }

    Score best = scorer.score(position);
    nodes_searched++;

    MovePicker movepicker(position, begin, end);

    while (movepicker.has_next())
    {
        Move move = movepicker.get_next();

        if (position.piece_at(to(move)) == NO_PIECE &&
                promotion(move) == NO_PIECE_KIND &&
                !is_in_check)
            continue;

        MoveInfo moveinfo = position.do_move(move);
        Score result = -quiescence_search(position, depth - 1, -beta, -alpha);
        position.undo_move(move, moveinfo);

        best = std::max(best, result);
        alpha = std::max(alpha, result);
        if (alpha >= beta)
            break;
    }

    return best;
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

    if (thinking_time > 0 && elapsed >= thinking_time)
    {
        stop_search = true;
        return true;
    }

    return false;
}

}
