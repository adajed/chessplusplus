#include "move_picker.h"
#include "search.h"
#include "transposition_table.h"

#include <chrono>
#include <cstring>

namespace engine
{

Score mate_in(int ply)
{
    return INFINITY_SCORE - ply;
}

bool is_score_mate(Score score)
{
    return (score < (-INFINITY_SCORE + MAX_DEPTH)) ||
           (score > (INFINITY_SCORE - MAX_DEPTH));
}

Search::Search(const PositionScorer& scorer)
    : check_limits_counter(4096)
    , stop_search(false)
    , scorer(scorer)
    , thinking_time(120LL * 1000LL)
{
}

void Search::stop()
{
    stop_search = true;
}

Move Search::select_move(const Position& position, int depth)
{
    Position pos = position;
    stop_search = false;

    if (depth > 0)
    {
        search(pos, depth, -INFINITY_SCORE, INFINITY_SCORE, pv_moves);
    }
    else
    {
        start_time = std::chrono::steady_clock::now();
        TimePoint end_time = start_time;
        int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        Move *my_pv = new Move[MAX_DEPTH];

        depth = 1;
        while (elapsed < (thinking_time / 2) && !stop_search)
        {
            depth++;
            Score result = search(pos, depth, -INFINITY_SCORE, INFINITY_SCORE, my_pv);

            if (!stop_search)
                std::memcpy(pv_moves, my_pv, depth * sizeof(Move));

            end_time = std::chrono::steady_clock::now();
            elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

            if (is_score_mate(result))
                break;

        }

        delete [] my_pv;
    }

    return pv_moves[0];
}

void Search::set_thinking_time(uint64_t time)
{
    thinking_time = time;
}

uint64_t Search::get_thinking_time()
{
    return thinking_time;
}

int64_t Search::search(Position& position, int depth, int64_t alpha, int64_t beta, Move* pv_moves)
{
    if (stop_search || check_limits())
    {
        stop_search = true;
        return 0;
    }

    if (depth == 0)
        return quiescence_search(position, MAX_DEPTH - 1, alpha, beta);

    Move* begin = MOVE_LIST[depth];
    Move* end = generate_moves(position, position.side_to_move(), begin);

    if (begin == end)
    {
        if (position.is_in_check(position.side_to_move()))
            return -mate_in(depth);
        return DRAW_SCORE;
    }

    int64_t best = -INFINITY_SCORE;
    Move* my_pv = new Move[depth - 1];

    MovePicker movepicker(position, begin, end);

    while (movepicker.has_next())
    {
        Move move = movepicker.get_next();
        MoveInfo moveinfo = position.do_move(move);

        int64_t result = -search(position, depth - 1, -beta, -alpha, my_pv);

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

int64_t Search::quiescence_search(Position& position, int depth, int64_t alpha, int64_t beta)
{
    if (stop_search || check_limits())
    {
        stop_search = true;
        return 0;
    }

    if (depth <= 0)
        return scorer.score(position);

    Move* begin = QUIESCENCE_MOVE_LIST[depth];
    Move* end = generate_moves(position, position.side_to_move(), begin);

    bool is_in_check = position.is_in_check(position.side_to_move());

    if (begin == end)
    {
        if (is_in_check)
            return -INFINITY_SCORE;
        return DRAW_SCORE;
    }

    int64_t best = -INFINITY_SCORE;

    MovePicker movepicker(position, begin, end);

    while (movepicker.has_next())
    {
        Move move = movepicker.get_next();

        if (position.piece_at(to(move)) == NO_PIECE &&
                promotion(move) == NO_PIECE_KIND &&
                !is_in_check)
            continue;

        MoveInfo moveinfo = position.do_move(move);
        int64_t result = -quiescence_search(position, depth - 1, -beta, -alpha);
        position.undo_move(move, moveinfo);

        best = std::max(best, result);
        alpha = std::max(alpha, result);
        if (alpha >= beta)
            break;
    }

    if (best == -INFINITY_SCORE)
        best = scorer.score(position);

    return best;
}

bool Search::check_limits()
{
    if (--check_limits_counter > 0)
        return false;

    check_limits_counter = 4096;

    TimePoint end_time = std::chrono::steady_clock::now();
    int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    if (elapsed >= thinking_time)
    {
        stop_search = true;
        return true;
    }

    return false;
}

}
