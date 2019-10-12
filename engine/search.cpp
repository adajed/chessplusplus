#include "search.h"
#include "move_picker.h"

#include <chrono>

namespace engine
{

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

uint64_t duration(TimePoint start, TimePoint end)
{
    return std::chrono::duration_cast<
        std::chrono::microseconds>(end - start).count();
}

Search::Search(const PositionScorer& scorer)
    : scorer(scorer), thinking_time(120ULL * 1000000ULL)
{
}

Move Search::select_move(const Position& position, int depth)
{
    Position pos = position;

    ScoredMove move;
    if (depth > 0)
    {
        move = search(pos, depth, LOST, WIN);
    }
    else
    {
        TimePoint start_time = std::chrono::steady_clock::now();
        TimePoint end_time = start_time;
        uint64_t time = duration(start_time, end_time);

        depth = 1;
        while (time < thinking_time / 2)
        {
            move = search(pos, depth, LOST, WIN);
            end_time = std::chrono::steady_clock::now();
            time = duration(start_time, end_time);
        }
    }

    return move.move;
}

void Search::set_thinking_time(uint64_t time)
{
    thinking_time = time;
}

uint64_t Search::get_thinking_time()
{
    return thinking_time;
}

ScoredMove Search::search(Position& position, int depth, int64_t alpha, int64_t beta)
{
    if (depth == 0)
        return quiescence_search(position, MAX_DEPTH, alpha, beta);

    Move* begin = MOVE_LIST[depth];
    Move* end = generate_moves(position, begin);

    if (begin == end)
    {
        if (is_in_check(position))
            return {NO_MOVE, LOST};
        return {NO_MOVE, DRAW};
    }

    MovePicker movepicker(position, begin, end);

    ScoredMove best = {NO_MOVE, 2LL * LOST};

    while (movepicker.has_next())
    {
        Move move = movepicker.get_next();
        MoveInfo moveinfo = do_move(position, move);

        int64_t result = -search(position, depth - 1, -beta, -alpha).score;

        undo_move(position, move, moveinfo);

        if (result > best.score)
            best = {move, result};
        alpha = result > alpha ? result : alpha;
        if (alpha >= beta)
            break;
    }

    assert(best.move != NO_MOVE);

    return best;
}

ScoredMove Search::quiescence_search(Position& position, int depth, int64_t alpha, int64_t beta)
{
    assert(depth >= 0);

    Move* begin = QUIESCENCE_MOVE_LIST[depth];
    Move* end = generate_quiescence_moves(position, begin);

    if (begin == end)
        return {NO_MOVE, scorer.score(position)};

    MovePicker movepicker(position, begin, end);

    ScoredMove best = {NO_MOVE, 2LL * LOST};

    while (movepicker.has_next())
    {
        Move move = movepicker.get_next();
        MoveInfo moveinfo = do_move(position, move);

        int64_t result = -quiescence_search(position, depth - 1, -beta, -alpha).score;

        undo_move(position, move, moveinfo);

        if (result > best.score)
            best = {move, result};
        alpha = result > alpha ? result : alpha;
        if (alpha >= beta)
            break;
    }

    assert(best.move != NO_MOVE);

    return best;
}

}
