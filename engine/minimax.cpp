#include "minimax.h"

namespace engine
{

const int64_t MIN_VALUE = -(1ULL << 16);
const int64_t MAX_VALUE = 1ULL << 16;

const int64_t PAWN_POSITION_VALUE[] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    60, 70, 80, 90, 90, 80, 70, 60,
    50, 60, 70, 80, 80, 70, 60, 50,
    35, 45, 50, 60, 60, 50, 45, 35,
    20, 20, 40, 60, 60, 40, 20, 20,
    20, 20, 20, 40, 40, 20, 20, 20,
    30, 30, 30, 10, 10, 30, 30, 30,
    0,  0,  0,  0,  0,  0,  0,  0,
};

const int64_t KNIGHT_POSITION_VALUE[] = {
    -50, -40, -30, -20, -20, -30, -40, -50,
    -40, -30,  20,  20,  20,  20, -30, -40,
    -30,  10,  30,  40,  40,  30,  10, -30,
    -15,  20,  40,  60,  60,  40,  20, -15,
    -15,  20,  40,  60,  60,  40,  20, -15,
    -30,  10,  30,  40,  40,  30,  10, -30,
    -40, -30,  20,  20,  20,  20, -30, -40,
    -50, -40, -30  -20, -20, -30, -40, -50,
};

const int64_t BISHOP_POSITION_VALUE[] = {
    -50, -40,  50, -20, -20,  50, -40, -50,
    -40,  60,  50,  40,  40,  50,  60, -40,
     50,  50,  40,  40,  40,  40,  50,  50,
     30,  40,  40,  30,  30,  40,  40,  30,
     30,  40,  40,  30,  30,  40,  40,  30,
     50,  50,  40,  40,  40,  40,  50,  50,
    -40,  60,  50,  40,  40,  50,  60, -40,
    -50, -40,  50  -20, -20,  50, -40, -50,
};

const int64_t QUEEN_POSITION_VALUE[] = {
    20,  20,  20,  20,  20,  20,  20, 20,
    20,  30,  20,  20,  20,  20,  30, 20,
    20,  10,  30,  40,  40,  30,  10, 20,
    25,  20,  40,  60,  60,  40,  20, 25,
    25,  20,  40,  60,  60,  40,  20, 25,
    20,  10,  30,  40,  40,  30,  10, 20,
    20,  30,  20,  20,  20,  20,  30, 20,
    20,  20,  20,  20,  20,  20,  20, 20,
};

const int64_t KING_POSITION_VALUE[] = {
     50,  60,  40, -10, -10,  30,  60,  50,
     30,  30,   0, -30, -30,   0,  30,  30,
    -30, -30, -50, -50, -50, -50, -30, -30,
    -70, -70, -70, -70, -70, -70, -70, -70,
    -70, -70, -70, -70, -70, -70, -70, -70,
    -30, -30, -50, -50, -50, -50, -30, -30,
     30,  30,   0, -30, -30,   0,  30,  30,
     50,  60,  40, -10, -10,  30,  60,  50,
};

const int64_t QUEEN_VALUE  = 900;
const int64_t ROOK_VALUE   = 500;
const int64_t BISHOP_VALUE = 300;
const int64_t KNIGHT_VALUE = 300;
const int64_t PAWN_VALUE   = 100;

template <Color side>
Bitboard single_pawn_attack(const Position& pos)
{
    const Direction UPRIGHT = side == WHITE ? NORTHEAST : SOUTHWEST;
    const Direction UPLEFT  = side == WHITE ? NORTHWEST : SOUTHEAST;

    Bitboard pawns = pieces_bb(pos, side, PAWN);

    return shift<UPRIGHT>(pawns) | shift<UPLEFT>(pawns);
}

template <Color side>
Bitboard double_pawn_attack(const Position& pos)
{
    const Direction UPRIGHT = side == WHITE ? NORTHEAST : SOUTHWEST;
    const Direction UPLEFT  = side == WHITE ? NORTHWEST : SOUTHEAST;

    Bitboard pawns = pieces_bb(pos, side, PAWN);

    return shift<UPRIGHT>(pawns) & shift<UPLEFT>(pawns);
}


template <Color side>
int64_t score_side(const Position& pos)
{
    const Piece PAWN_ = make_piece(side, PAWN);
    const Piece KNIGHT_ = make_piece(side, KNIGHT);
    const Piece BISHOP_ = make_piece(side, BISHOP);
    const Piece ROOK_ = make_piece(side, ROOK);
    const Piece QUEEN_ = make_piece(side, QUEEN);
    const Piece KING_  = make_piece(side, KING);

    int64_t value = 0;
    value += QUEEN_VALUE * pos.piece_count[QUEEN_];
    value += ROOK_VALUE * pos.piece_count[ROOK_];
    value += BISHOP_VALUE * pos.piece_count[BISHOP_];
    value += KNIGHT_VALUE * pos.piece_count[KNIGHT_];
    value += PAWN_VALUE * pos.piece_count[PAWN_];

    for (int i = 0; i < pos.piece_count[PAWN_]; ++i)
    {
        Square sq = pos.piece_position[PAWN_][i];
        assert(sq != NO_SQUARE);
        value += PAWN_POSITION_VALUE[side == WHITE ? 63 - sq : sq];
    }

    for (int i = 0; i < pos.piece_count[KNIGHT_]; ++i)
    {
        Square sq = pos.piece_position[KNIGHT_][i];
        assert(sq != NO_SQUARE);
        value += KNIGHT_POSITION_VALUE[side == WHITE ? 63 - sq : sq];
    }

    for (int i = 0; i < pos.piece_count[BISHOP_]; ++i)
    {
        Square sq = pos.piece_position[BISHOP_][i];
        assert(sq != NO_SQUARE);
        value += BISHOP_POSITION_VALUE[side == WHITE ? 63 - sq : sq];
    }

    for (int i = 0; i < pos.piece_count[QUEEN_]; ++i)
    {
        Square sq = pos.piece_position[QUEEN_][i];
        assert(sq != NO_SQUARE);
        value += QUEEN_POSITION_VALUE[side == WHITE ? 63 - sq : sq];
    }

    for (int i = 0; i < pos.piece_count[KING_]; ++i)
    {
        Square sq = pos.piece_position[KING_][i];
        assert(sq != NO_SQUARE);
        value += QUEEN_POSITION_VALUE[side == WHITE ? 63 - sq : sq];
    }


    Bitboard single_attack = single_pawn_attack<side>(pos);
    value += 20 * popcount(single_attack);

    Bitboard double_attack = single_pawn_attack<side>(pos);
    value += 20 * popcount(double_attack);

    return value;
}

int64_t score(const Position& pos)
{
    return score_side<WHITE>(pos) - score_side<BLACK>(pos);
}

namespace
{

ScoredMove minimax_with_alpha_beta_prunning(
        Position& position, int64_t alpha, int64_t beta, int depth)
{
    Color side = position.current_side;

    if (depth == 0)
        return {{}, score(position)};

    Move* begin = MOVE_LIST[depth];
    Move* end = generate_moves(position, begin);

#ifdef DEBUG
    std::cout << "depth " << depth << std::endl;
    std::cout << position << std::endl;

    std::cout << "Availables moves (" << end - begin << "):" << std::endl;
    for (Move* it = begin; it != end; ++it)
        std::cout << *it << std::endl;
    std::cout << std::endl;
#endif

    if (begin == end)
    {
        if (!is_in_check(position))
            return {{}, 0};
        return {{}, side == WHITE ? MIN_VALUE : MAX_VALUE};
    }

    Move best_move{NO_SQUARE, NO_SQUARE, NO_PIECE_KIND, NO_PIECE_KIND,
                    NO_CASTLING, NO_CASTLING, NO_SQUARE, false};
    int64_t best_value;

    if (side == WHITE)
    {
        best_value = MIN_VALUE;
        for (Move* it = begin; it != end; ++it)
        {
            Move move = *it;
            do_move(position, move);
            int64_t value = minimax_with_alpha_beta_prunning(
                    position, alpha, beta, depth - 1).score;
            if (value > best_value)
            {
                best_value = value;
                best_move = move;
            }
            undo_move(position, move);

            alpha = alpha > value ? alpha : value;
            if (alpha >= beta)
                break;
        }
    }
    else
    {
        best_value = MAX_VALUE;
        for (Move* it = begin; it != end; ++it)
        {
            Move move = *it;
            do_move(position, move);
            int64_t value = minimax_with_alpha_beta_prunning(
                    position, alpha, beta, depth - 1).score;
            if (value < best_value)
            {
                best_value = value;
                best_move = move;
            }
            undo_move(position, move);

            beta = beta < value ? beta : value;
            if (alpha >= beta)
                break;
        }
    }

    return {best_move, best_value};

}

ScoredMove minimax_without_alpha_beta_prunning(
        Position& position, int depth)
{
    Color side = position.current_side;

    if (depth == 0)
        return {{}, score(position)};

    Move* begin = MOVE_LIST[depth];
    Move* end = generate_moves(position, begin);

#ifdef DEBUG
    std::cout << "depth " << depth << std::endl;
    std::cout << position << std::endl;

    std::cout << "Availables moves (" << end - begin << "):" << std::endl;
    for (Move* it = begin; it != end; ++it)
        std::cout << *it << std::endl;
    std::cout << std::endl;
#endif

    if (begin == end)
    {
        if (!is_in_check(position))
            return {{}, 0};
        return {{}, side == WHITE ? MIN_VALUE : MAX_VALUE};
    }

    Move best_move;
    int64_t best_value;

    if (side == WHITE)
    {
        best_value = MIN_VALUE;
        for (Move* it = begin; it != end; ++it)
        {
            Move move = *it;
            do_move(position, move);
            int64_t value = minimax_without_alpha_beta_prunning(position, depth - 1).score;
            if (value > best_value)
            {
                best_value = value;
                best_move = move;
            }
            undo_move(position, move);

        }
    }
    else
    {
        best_value = MAX_VALUE;
        for (Move* it = begin; it != end; ++it)
        {
            Move move = *it;
            do_move(position, move);
            int64_t value = minimax_without_alpha_beta_prunning(position, depth - 1).score;
            if (value < best_value)
            {
                best_value = value;
                best_move = move;
            }
            undo_move(position, move);
        }
    }

    return {best_move, best_value};
}

}

ScoredMove minimax(Position& position, int depth, bool use_alpha_beta_prunning)
{
    if (use_alpha_beta_prunning)
        return minimax_with_alpha_beta_prunning(
                position, -(1ULL << 32), 1ULL << 32, depth);
    return minimax_without_alpha_beta_prunning(
            position, depth);
}

uint64_t perft(Position& position, int depth, bool print_moves)
{
    if (depth == 0)
        return 1;

    Move* begin = MOVE_LIST[depth];
    Move* end = generate_moves(position, begin);

    if (depth == 1)
    {
        if (print_moves)
        {
            for (Move* it = begin; it != end; ++it)
                std::cout << *it << " 1" << std::endl;
        }
        return end - begin;
    }

    uint64_t sum = 0;
    for (Move* it = begin; it != end; ++it)
    {
        Move move = *it;
        do_move(position, move);

        uint64_t nodes = perft(position, depth - 1, false);
        sum += nodes;

        if (print_moves)
        {
            std::cout << move << " " << nodes << std::endl;
            /* std::cout << position << std::endl; */
        }

        undo_move(position, move);
    }

    return sum;
}

}  // namespace engine
