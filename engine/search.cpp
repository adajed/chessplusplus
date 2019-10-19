#include "move_picker.h"
#include "search.h"
#include "transposition_table.h"

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

    ScoredMove move = {NO_MOVE, LOST};

    if (depth > 0)
    {
        move = search(pos, depth, LOST, WIN);
    }
    else
    {
        TimePoint start_time = std::chrono::steady_clock::now();
        TimePoint end_time = start_time;
        uint64_t time = duration(start_time, end_time);

        depth = 3;
        while (time < thinking_time / 2)
        {
            depth++;
            move = search(pos, depth, LOST, WIN);
            end_time = std::chrono::steady_clock::now();
            time = duration(start_time, end_time);

            std::cout << "[depth " << depth << "] "
                      << "time=" << time / 1000000ULL << "s "
                      << "move=";
            print_move(std::cout, move.move);
            std::cout << " score=" << move.score << std::endl;
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
        return quiescence_search(position, MAX_DEPTH - 1, alpha, beta);

    Move* begin = MOVE_LIST[depth];
    Move* end = generate_moves(position, position.side_to_move(), begin);

    if (begin == end)
    {
        if (position.is_in_check(position.side_to_move()))
            return {NO_MOVE, LOST};
        return {NO_MOVE, DRAW};
    }

    ScoredMove best = {NO_MOVE, 2LL * LOST};

    /*
    if (transposition::contains(position.zobrist_hash))
    {
        transposition::Entry entry = transposition::get(position.zobrist_hash);
        if (entry.position == position)
        {
            if (entry.depth >= depth)
            {
                best = {entry.best_move, entry.value};

                alpha = best.score > alpha ? best.score : alpha;
                if (alpha >= beta)
                    return best;
            }
            else
            {
                MoveInfo moveinfo = do_move(position, entry.best_move);
                best = {entry.best_move, -search(position, depth - 1, -beta, -alpha).score};
                undo_move(position, entry.best_move, moveinfo);

                alpha = best.score > alpha ? best.score : alpha;
                if (alpha >= beta)
                {
                    transposition::update(position.zobrist_hash, {position, best.move, depth, best.score});
                    return best;
                }
            }

        }
    }
    */

    MovePicker movepicker(position, begin, end);
    bool searchPV = true;

    while (movepicker.has_next())
    {
        Move move = movepicker.get_next();
        MoveInfo moveinfo = position.do_move(move);

        int64_t result;
        if (searchPV)
            result = -search(position, depth - 1, -beta, -alpha).score;
        else
        {
            result = -search(position, depth - 1, -(alpha + 1), -alpha).score;
            if (result > alpha)
                result = -search(position, depth - 1, -beta, -alpha).score;
        }

        position.undo_move(move, moveinfo);

        if (result >= beta)
            return {best.move, beta};
        if (result > alpha)
        {
            best = {move, result};
            alpha = result;
            searchPV = false;
        }
    }

    return best;
}

ScoredMove Search::quiescence_search(Position& position, int depth, int64_t alpha, int64_t beta)
{
    assert(depth >= 0);

    Move* begin = QUIESCENCE_MOVE_LIST[depth];
    Move* end = generate_quiescence_moves(position, position.side_to_move(), begin);

    if (begin == end)
        return {NO_MOVE, scorer.score(position)};

    ScoredMove best = {NO_MOVE, 2LL * LOST};

    /* if (transposition::contains(position.zobrist_hash)) */
    /* { */
    /*     transposition::Entry entry = transposition::get(position.zobrist_hash); */
    /*     if (entry.position == position) */
    /*     { */
    /*         if (entry.depth >= depth) */
    /*         { */
    /*             best = {entry.best_move, entry.value}; */
    /*             alpha = best.score > alpha ? best.score : alpha; */
    /*             if (alpha >= beta) */
    /*                 return best; */
    /*         } */
    /*     } */
    /* } */

    MovePicker movepicker(position, begin, end);

    while (movepicker.has_next())
    {
        Move move = movepicker.get_next();
        MoveInfo moveinfo = position.do_move(move);
        int64_t result = -quiescence_search(position, depth - 1, -beta, -alpha).score;
        position.undo_move(move, moveinfo);

        if (result > best.score)
            best = {move, result};
        alpha = result > alpha ? result : alpha;
        if (alpha >= beta)
            break;
    }

    return best;
}

}
