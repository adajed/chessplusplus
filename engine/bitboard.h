#ifndef CHESS_ENGINE_BITBOARD_H_
#define CHESS_ENGINE_BITBOARD_H_

#include "types.h"

#include <cassert>

namespace engine
{
constexpr Bitboard no_squares_bb = 0ULL;
constexpr Bitboard all_squares_bb = ~no_squares_bb;
constexpr Bitboard white_squares_bb = 0x55aa55aa55aa55aaULL;
constexpr Bitboard black_squares_bb = ~white_squares_bb;

const Bitboard color_squares[COLOR_NUM] = {white_squares_bb, black_squares_bb};

constexpr Bitboard rank1_bb = 0x00000000000000ffULL;
constexpr Bitboard rank2_bb = rank1_bb << (8 * 1);
constexpr Bitboard rank3_bb = rank1_bb << (8 * 2);
constexpr Bitboard rank4_bb = rank1_bb << (8 * 3);
constexpr Bitboard rank5_bb = rank1_bb << (8 * 4);
constexpr Bitboard rank6_bb = rank1_bb << (8 * 5);
constexpr Bitboard rank7_bb = rank1_bb << (8 * 6);
constexpr Bitboard rank8_bb = rank1_bb << (8 * 7);
const Bitboard RANKS_BB[RANK_NUM] = {rank1_bb, rank2_bb, rank3_bb, rank4_bb,
                                     rank5_bb, rank6_bb, rank7_bb, rank8_bb};

/**
 * @brief Checks if bitboard is contained in single rank
 */
constexpr bool all_on_same_rank(Bitboard bb)
{
    return bb == (bb & rank1_bb) || bb == (bb & rank2_bb)
        || bb == (bb & rank3_bb) || bb == (bb & rank4_bb)
        || bb == (bb & rank5_bb) || bb == (bb & rank6_bb)
        || bb == (bb & rank7_bb) || bb == (bb & rank8_bb);
}

/*
 * Bitboard containing ranks of opponent
 */
const Bitboard OPPONENT_RANKS[COLOR_NUM] = {
    rank5_bb | rank6_bb | rank7_bb | rank8_bb,
    rank1_bb | rank2_bb | rank3_bb | rank4_bb};

constexpr Bitboard middle_ranks =
    rank2_bb | rank3_bb | rank4_bb | rank5_bb | rank6_bb | rank7_bb;

constexpr Bitboard fileA_bb = 0x0101010101010101ULL;
constexpr Bitboard fileB_bb = fileA_bb << 1;
constexpr Bitboard fileC_bb = fileA_bb << 2;
constexpr Bitboard fileD_bb = fileA_bb << 3;
constexpr Bitboard fileE_bb = fileA_bb << 4;
constexpr Bitboard fileF_bb = fileA_bb << 5;
constexpr Bitboard fileG_bb = fileA_bb << 6;
constexpr Bitboard fileH_bb = fileA_bb << 7;
const Bitboard FILES_BB[FILE_NUM] = {fileA_bb, fileB_bb, fileC_bb, fileD_bb,
                                     fileE_bb, fileF_bb, fileG_bb, fileH_bb};

/**
 * @brief Checks if bitboard is contained in single file
 */
constexpr bool all_on_same_file(Bitboard bb)
{
    return bb == (bb & fileA_bb) || bb == (bb & fileB_bb)
        || bb == (bb & fileC_bb) || bb == (bb & fileD_bb)
        || bb == (bb & fileE_bb) || bb == (bb & fileF_bb)
        || bb == (bb & fileG_bb) || bb == (bb & fileH_bb);
}

const Bitboard NEIGHBOUR_FILES_BB[FILE_NUM] = {fileB_bb,
                                               fileA_bb | fileC_bb,
                                               fileB_bb | fileD_bb,
                                               fileC_bb | fileE_bb,
                                               fileD_bb | fileF_bb,
                                               fileE_bb | fileG_bb,
                                               fileF_bb | fileH_bb,
                                               fileG_bb};

constexpr Bitboard center_bb = (fileD_bb | fileE_bb) & (rank4_bb & rank5_bb);

/*
 * Returns bb with ranks in front of the given square
 * (from the persepctive of side).
 */
template <Color side>
inline Bitboard forward_ranks_bb(Square square)
{
    return side == WHITE ? ~rank1_bb << 8 * (rank(square) - RANK_1)
                         : ~rank8_bb >> 8 * (RANK_8 - rank(square));
}

template <Color side>
inline Bitboard passed_pawn_bb(Square square)
{
    return forward_ranks_bb<side>(square) &
           (NEIGHBOUR_FILES_BB[file(square)] | FILES_BB[file(square)]);
}

template <Color side>
inline Bitboard squares_left_behind_bb(Square square)
{
    return NEIGHBOUR_FILES_BB[file(square)] &
           (forward_ranks_bb<!side>(square) | RANKS_BB[rank(square)]);
}

enum Direction : int32_t
{
    NORTH = 8,
    EAST = 1,
    SOUTH = -NORTH,
    WEST = -EAST,

    NORTHWEST = NORTH + WEST,
    NORTHEAST = NORTH + EAST,
    SOUTHWEST = SOUTH + WEST,
    SOUTHEAST = SOUTH + EAST,

    DOUBLENORTH = NORTH + NORTH,
    DOUBLESOUTH = SOUTH + SOUTH
};

template <Direction dir>
Bitboard shift(Bitboard bb)
{
    // clang-format off
    return dir == NORTH         ? bb << 8 :
           dir == EAST          ? (bb & ~fileH_bb) << 1 :
           dir == SOUTH         ? bb >> 8 :
           dir == WEST          ? (bb & ~fileA_bb) >> 1 :
           dir == NORTHEAST     ? (bb & ~fileH_bb) << 9 :
           dir == NORTHWEST     ? (bb & ~fileA_bb) << 7 :
           dir == SOUTHEAST     ? (bb & ~fileH_bb) >> 7 :
           dir == SOUTHWEST     ? (bb & ~fileA_bb) >> 9 :
           dir == DOUBLENORTH   ? bb << 16 :
           dir == DOUBLESOUTH   ? bb >> 16 :
           0ULL;
    // clang-format on
}

inline Bitboard shift(Bitboard bb, Direction dir)
{
    switch (dir)
    {
    case NORTH: return shift<NORTH>(bb);
    case EAST: return shift<EAST>(bb);
    case SOUTH: return shift<SOUTH>(bb);
    case WEST: return shift<WEST>(bb);
    case NORTHEAST: return shift<NORTHEAST>(bb);
    case NORTHWEST: return shift<NORTHWEST>(bb);
    case SOUTHEAST: return shift<SOUTHEAST>(bb);
    case SOUTHWEST: return shift<SOUTHWEST>(bb);
    case DOUBLENORTH: return shift<DOUBLENORTH>(bb);
    case DOUBLESOUTH: return shift<DOUBLESOUTH>(bb);
    }
    assert(false);
    return 0ULL;
}

template <Color side>
Bitboard pawn_attacks(Bitboard bb)
{
    return side == WHITE ? (shift<NORTHWEST>(bb) | shift<NORTHEAST>(bb))
                         : (shift<SOUTHWEST>(bb) | shift<SOUTHEAST>(bb));
}

inline Bitboard pawn_attacks(Bitboard bb, Color side)
{
    return side == WHITE ? pawn_attacks<WHITE>(bb)
                         : pawn_attacks<BLACK>(bb);
}

/*
 * Returns bb with squares that are attacked by at least two pawns.
 * \input bb Bitboard with position of pawns.
 */
template <Color side>
Bitboard pawn_doubled_attacks(Bitboard bb)
{
    return side == WHITE ? (shift<NORTHWEST>(bb) & shift<NORTHEAST>(bb))
                         : (shift<SOUTHWEST>(bb) & shift<SOUTHEAST>(bb));
}

inline Bitboard king_attacks(Bitboard bb)
{
    // clang-format off
    return shift<NORTH    >(bb)
         | shift<SOUTH    >(bb)
         | shift<WEST     >(bb)
         | shift<EAST     >(bb)
         | shift<NORTHEAST>(bb)
         | shift<NORTHWEST>(bb)
         | shift<SOUTHEAST>(bb)
         | shift<SOUTHWEST>(bb);
    // clang-format on
}

}  // namespace engine

#endif  // CHESS_ENGINE_BITBOARD_H_
