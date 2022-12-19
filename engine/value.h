#ifndef CHESS_ENGINE_VALUE_H_
#define CHESS_ENGINE_VALUE_H_

#include "types.h"
#include <cstdint>
#include <iomanip>

namespace engine
{

using Value = int64_t;

constexpr Value BASIC_PIECE_VALUE[PIECE_KIND_NUM] = {
    0, 100, 300, 300, 500, 900, 0
};

constexpr Value ALL_PIECE_VALUE = 2 * (8 * BASIC_PIECE_VALUE[PAWN] +
                                       2 * BASIC_PIECE_VALUE[KNIGHT] +
                                       2 * BASIC_PIECE_VALUE[BISHOP] +
                                       2 * BASIC_PIECE_VALUE[ROOK] +
                                       1 * BASIC_PIECE_VALUE[QUEEN]);

constexpr Value VALUE_DRAW = 0LL;
constexpr Value VALUE_NONE = 640'002LL;
constexpr Value VALUE_INFINITE = VALUE_NONE - 1;
constexpr Value VALUE_MATE = VALUE_INFINITE - 1;
constexpr Value VALUE_KNOWN_WIN = VALUE_MATE - 8 * MAX_DEPTH - 8 * ALL_PIECE_VALUE;

static_assert(ALL_PIECE_VALUE < VALUE_KNOWN_WIN);

/**
 * @brief Value corresponding to known mate (win) in given ply.
 */
constexpr Value win_in(int ply)
{
    return VALUE_MATE - ply;
}

/**
 * @brief Value corresponding to known mate (loss) in given ply.
 */
constexpr Value lost_in(int ply)
{
    return -win_in(ply);
}

constexpr bool is_mate(Value score)
{
    ASSERT(score != VALUE_NONE && score != VALUE_INFINITE && score != -VALUE_INFINITE);
    return score <= lost_in(MAX_DEPTH) || score >= win_in(MAX_DEPTH);
}

struct Score
{
    Value mg, eg;

    Score() : mg(0), eg(0) {}
    Score(Value mg, Value eg) : mg(mg), eg(eg) {}
    explicit Score(Value v) : mg(v), eg(v) {}
    Score(const Score& other) = default;
    Score(Score&& other) = default;

    Score& operator=(const Score& other) = default;
    Score& operator=(Score&& other) = default;

    Score operator+(const Score& other) const
    {
        return Score(mg + other.mg, eg + other.eg);
    }
    Score operator-(const Score& other) const
    {
        return Score(mg - other.mg, eg - other.eg);
    }

    Score& operator+=(const Score& other)
    {
        mg += other.mg;
        eg += other.eg;

        return *this;
    }
};

inline Score operator*(const Score& score, const Value& value)
{
    return Score(score.mg * value, score.eg * value);
}

inline Score operator*(const Value& value, const Score& score)
{
    return Score(score.mg * value, score.eg * value);
}

inline std::ostream& operator<< (std::ostream& os, Score s)
{
    return os << std::setw(6) << std::setfill(' ') << s.mg << " "
              << std::setw(6) << std::setfill(' ') << s.eg << " ";
}

#define S(mg, eg) (Score((mg), (eg)))

const Score PIECE_VALUE[PIECE_KIND_NUM] = {
    //     ,       pawn ,     knight ,     bishop ,         rook ,        queen ,     king
    S(0, 0), S(300, 370), S(890, 890), S(950, 900), S(1500, 1500), S(2700, 2700), S(0, 0)};

const Score MOBILITY_BONUS[PIECE_KIND_NUM] = {
    S(0, 0), S(5, 10), S(6, 12), S(9, 4), S(2, 12), S(2, 6), S(0, 5)};

const Score PINNED_PENALTY[PIECE_KIND_NUM] = {
    S(0, 0), S(-20, -20), S(-10, -15), S(-10, -20), S(0, 0), S(0, 0)};

// bonus for rook on semiopen file
const Score ROOK_SEMIOPEN_FILE_BONUS = S(10, 11);

// bonus for rook on open file
const Score ROOK_OPEN_FILE_BONUS = S(20, 40);

const Score TRAPPED_ROOK_PENALTY = S(-50, -10);

// bonus for bishop pair
const Score BISHOP_PAIR_BONUS = S(50, 60);

// bonus for passed pawn
const Score PASSED_PAWN_BONUS = S(20, 40);

// bonus for connecting rooks
const Score CONNECTED_ROOKS_BONUS = S(20, 10);

const Score OUTPOST_BONUS = S(20, 10);

const Score OUTPOST_KNIGHT_BONUS = S(25, 10);

const Score OUTPOST_BISHOP_BONUS = S(20, 10);

// penalty for double pawns
const Score DOUBLE_PAWN_PENALTY = S(-15, -45);

// penalty for tripled pawns
const Score TRIPLE_PAWN_PENALTY = S(-35, -100);

const Score PAWN_CHAIN_BONUS[FILE_NUM] = {S(0, 0),  S(0, 0),  S(2, 4),
                                          S(4, 10), S(5, 15), S(5, 20),
                                          S(6, 20), S(7, 20)};

const Value CONNECTED_PAWNS_BONUS[FILE_NUM] = {Value(0),  Value(0),  Value(2),
                                               Value(5),  Value(20), Value(40),
                                               Value(80), Value(0)};

const Score BACKWARD_PAWN_PENALTY = S(-30, -100);

const Score ISOLATED_PAWN_PENALTY = S(-20, -80);

const Score KING_SAFETY_BONUS = S(30, 0);
const Score SAFE_KNIGHT = S(10, 2);
const Score CONTROL_CENTER_KNIGHT = S(10, 10);

const Score CONTROL_SPACE[PIECE_KIND_NUM] = {S(20, 30), S(0, 0),   S(10, 5),
                                             S(10, 10), S(10, 20), S(0, 0)};

const Score KING_PROTECTOR_PENALTY[PIECE_KIND_NUM] = {
    S(0, 0), S(-6, -4), S(-5, -3), S(0, 0), S(0, 0), S(0, 0)};

const Score KING_ATTACKER_PENALTY[PIECE_KIND_NUM] = {
    S(0, 0), S(-7, -4), S(-4, -3), S(0, 0), S(0, 0), S(0, 0)};

const Score PAWN_ISLAND_PENALTY = S(-10, -20);

const Score VULNERABLE_QUEEN_PENALTY = S(-30, -15);

const Score WEAK_BACKRANK_PENALTY = S(-50, -50);

const Score WEAK_KING_RAYS_PENALTY = S(-15, -20);

const Score WEAK_KING_DIAGONALS = S(-5, 0);
const Score WEAK_KING_LINES = S(-7, 0);

const Score KING_PAWN_PROXIMITY_PENALTY = S(0, -5);

const Score PAWNS_ON_SAME_COLOR_AS_BISHOP_PENALTY = S(-3, -5);

#undef S


} /* namespace engine */

#endif /* end of include guard: CHESS_ENGINE_VALUE_H_ */
