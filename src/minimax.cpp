#include "minimax.h"

namespace engine
{

int32_t score(Position& pos)
{
    int32_t s = 0;

    const int32_t QUEEN_VALUE = 9;
    const int32_t ROOK_VALUE = 5;
    const int32_t BISHOP_VALUE = 3;
    const int32_t KNIGHT_VALUE = 3;
    const int32_t PAWN_VALUE = 1;

    s += QUEEN_VALUE * pos.piece_count[W_QUEEN];
    s += ROOK_VALUE * pos.piece_count[W_ROOK];
    s += BISHOP_VALUE * pos.piece_count[W_BISHOP];
    s += KNIGHT_VALUE * pos.piece_count[W_KNIGHT];
    s += PAWN_VALUE * pos.piece_count[W_PAWN];

    s -= QUEEN_VALUE * pos.piece_count[B_QUEEN];
    s -= ROOK_VALUE * pos.piece_count[B_ROOK];
    s -= BISHOP_VALUE * pos.piece_count[B_BISHOP];
    s -= KNIGHT_VALUE * pos.piece_count[B_KNIGHT];
    s -= PAWN_VALUE * pos.piece_count[B_PAWN];

    return s;
}

ScoredMove minimax(Position& pos, int depth)
{
    std::cout << "[" << depth << "] enter" << std::endl;
    std::cout << pos;
    Color side = pos.current_side;

    if (depth == 0)
    {
        std::cout << "[" << depth << "] leave with score " << score(pos) << std::endl;
        return {{}, score(pos)};
    }

    Move* begin =  MOVE_LIST[depth];
    Move* end = generate_moves(pos, begin);

    if (begin == end)
    {
        std::cout << "[" << depth << "] leave with score " << (1 << 16) * (side == WHITE ? -1 : 1) << std::endl;
        return {{}, (1 << 16) * (side == WHITE ? -1 : 1)};
    }

    Move best_move;
    int32_t best_value;

    if (side == WHITE)
    {
        best_value = -100;
        for (Move* it = begin; it != end; ++it)
        {
            std::cout << "[" << depth << "] " << "do move = " << *it << std::endl;
            do_move(pos, *it);
            int32_t value = minimax(pos, depth - 1).score;
            if (value > best_value)
            {
                best_value = value;
                best_move = *it;
            }
            std::cout << "[" << depth << "] " << "undo move = " << *it << std::endl;
            undo_move(pos, *it);
        }
    }
    else
    {
        best_value = 100;
        for (Move* it = begin; it != end; ++it)
        {
            std::cout << "[" << depth << "] " << "do move = " << *it << std::endl;
            do_move(pos, *it);
            int32_t value = minimax(pos, depth - 1).score;
            if (value < best_value)
            {
                best_value = value;
                best_move = *it;
            }
            std::cout << "[" << depth << "] " << "undo move = " << *it << std::endl;
            undo_move(pos, *it);
        }
    }

    std::cout << "[" << depth << "] leave with score " << best_value << std::endl;
    return {best_move, best_value};
}

}  // namespace engine
