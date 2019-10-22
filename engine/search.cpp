#include "move_picker.h"
#include "search.h"
#include "transposition_table.h"

#include <chrono>
#include <cstring>

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

    if (depth > 0)
    {
        search(pos, depth, LOST, WIN, pv_moves);
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
            int64_t result = search(pos, depth, LOST, WIN, pv_moves);
            end_time = std::chrono::steady_clock::now();
            time = duration(start_time, end_time);

            if (result == LOST || result == WIN)
                break;

            /* std::cout << "[depth " << depth << "] " */
            /*           << "time=" << time / 1000000ULL << "s " */
            /*           << "score=" << result << " " */
            /*           << "pv="; */
            /* for (int i = 0; i < depth; ++i) */
            /*     print_move(std::cout, pv_moves[i]) << " "; */
            /* std::cout << std::endl; */
        }
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
    if (depth == 0)
        return quiescence_search(position, MAX_DEPTH - 1, alpha, beta);

    Move* begin = MOVE_LIST[depth];
    Move* end = generate_moves(position, position.side_to_move(), begin);

    if (begin == end)
    {
        if (position.is_in_check(position.side_to_move()))
            return LOST;
        return DRAW;
    }

    int64_t best = 2LL * LOST;
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
    if (depth <= 0)
        return scorer.score(position);

    Move* begin = QUIESCENCE_MOVE_LIST[depth];
    Move* end = generate_moves(position, position.side_to_move(), begin);

    bool is_in_check = position.is_in_check(position.side_to_move());

    if (begin == end)
    {
        if (is_in_check)
            return LOST;
        return DRAW;
    }

    int64_t best = 2LL * LOST;

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

    if (best == 2LL * LOST)
        best = scorer.score(position);

    return best;
}

}
