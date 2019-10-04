#include "minimax.h"

namespace engine
{

const int32_t PAWN_POSITION_VALUE[] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    60, 70, 80, 90, 90, 80, 70, 60,
    50, 60, 70, 80, 80, 70, 60, 50,
    35, 45, 50, 60, 60, 50, 45, 35,
    30, 35, 40, 60, 60, 40, 35, 30,
    30, 30, 30, 40, 40, 30, 30, 30,
    30, 30, 30, 10, 10, 30, 30, 30,
    0,  0,  0,  0,  0,  0,  0,  0,
};

const int32_t KNIGHT_POSITION_VALUE[] = {
    -50, -40, -30, -20, -20, -30, -40, -50,
    -40, -30, 20,  20,  20,  20,  -30, -40,
    -30, 10, 30,  40,  40,  30,  10, -30,
    -15, 20, 40,  60,  60,  40,  20, -15,
    -15, 20, 40,  60,  60,  40,  20, -15,
    -30, 10, 30,  40,  40,  30,  10, -30,
    -40, -30, 20,  20,  20,  20,  -30, -40,
    -50, -40, -30  -20, -20, -30, -40, -50,
};

const int32_t QUEEN_VALUE  = 900;
const int32_t ROOK_VALUE   = 500;
const int32_t BISHOP_VALUE = 300;
const int32_t KNIGHT_VALUE = 300;
const int32_t PAWN_VALUE   = 100;


template <Color side>
int32_t score_side(const Position& pos)
{
    const Piece PAWN_ = make_piece(side, PAWN);
    const Piece KNIGHT_ = make_piece(side, KNIGHT);
    const Piece BISHOP_ = make_piece(side, BISHOP);
    const Piece ROOK_ = make_piece(side, ROOK);
    const Piece QUEEN_ = make_piece(side, QUEEN);

    int32_t value = 0;
    value += QUEEN_VALUE * pos.piece_count[QUEEN_];
    value += ROOK_VALUE * pos.piece_count[ROOK_];
    value += BISHOP_VALUE * pos.piece_count[BISHOP_];
    value += KNIGHT_VALUE * pos.piece_count[KNIGHT_];
    value += PAWN_VALUE * pos.piece_count[PAWN_];

    for (int i = 0; i < pos.piece_count[PAWN_]; ++i)
    {
        Square sq = pos.piece_position[PAWN_][i];
        value += PAWN_POSITION_VALUE[side == WHITE ? 63 - sq : sq];
    }

    for (int i = 0; i < pos.piece_count[KNIGHT_]; ++i)
    {
        Square sq = pos.piece_position[KNIGHT_][i];
        value += KNIGHT_POSITION_VALUE[side == WHITE ? 63 - sq : sq];
    }

    return value;
}

int32_t score(const Position& pos)
{
    return score_side<WHITE>(pos) - score_side<BLACK>(pos);
}

ScoredMove minimax(Position& pos, int depth)
{
#ifdef DEBUG
    std::cout << "[" << depth << "] enter" << std::endl;
    std::cout << pos;
#endif
    Color side = pos.current_side;

    if (depth == 0)
    {
#ifdef DEBUG
        std::cout << "[" << depth << "] leave with score " << score(pos) << std::endl;
#endif
        return {{}, score(pos)};
    }

    Move* begin =  MOVE_LIST[depth];
    Move* end = generate_moves(pos, begin);

    if (begin == end)
    {
#ifdef DEBUG
        std::cout << "[" << depth << "] leave with score " << (1 << 16) * (side == WHITE ? -1 : 1) << std::endl;
#endif
        return {{}, (1 << 16) * (side == WHITE ? -1 : 1)};
    }

    Move best_move;
    int32_t best_value;

    if (side == WHITE)
    {
        best_value = -100;
        for (Move* it = begin; it != end; ++it)
        {
#ifdef DEBUG
            std::cout << "[" << depth << "] " << "do move = " << *it << std::endl;
#endif
            do_move(pos, *it);
            int32_t value = minimax(pos, depth - 1).score;
            if (value > best_value)
            {
                best_value = value;
                best_move = *it;
            }
#ifdef DEBUG
            std::cout << "[" << depth << "] " << "undo move = " << *it << std::endl;
#endif
            undo_move(pos, *it);

#ifdef DEBUG
            std::cout << pos;
#endif
        }
    }
    else
    {
        best_value = 100;
        for (Move* it = begin; it != end; ++it)
        {
#ifdef DEBUG
            std::cout << "[" << depth << "] " << "do move = " << *it << std::endl;
#endif
            do_move(pos, *it);
            int32_t value = minimax(pos, depth - 1).score;
            if (value < best_value)
            {
                best_value = value;
                best_move = *it;
            }
#ifdef DEBUG
            std::cout << "[" << depth << "] " << "undo move = " << *it << std::endl;
#endif
            undo_move(pos, *it);

#ifdef DEBUG
            std::cout << pos;
#endif
        }
    }

#ifdef DEBUG
    std::cout << "[" << depth << "] leave with score " << best_value << std::endl;
#endif
    return {best_move, best_value};
}

uint32_t perft(Position& position, int depth)
{
    if (depth == 0)
        return 0;

    Move* begin = MOVE_LIST[depth];
    Move* end = generate_moves(position, begin);

    if (depth == 1)
        return end - begin;


    uint32_t sum = 0;
    for (Move* it = begin; it != end; ++it)
    {
        do_move(position, *it);
        sum += perft(position, depth - 1);
        undo_move(position, *it);
    }

    return sum;
}

}  // namespace engine
