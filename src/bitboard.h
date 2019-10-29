#ifndef CHESS_ENGINE_BITBOARD_H_
#define CHESS_ENGINE_BITBOARD_H_

#include <cassert>

#include "types.h"

namespace engine
{

constexpr Bitboard all_squares_bb = ~Bitboard(0);
constexpr Bitboard white_squares_bb = 0x55aa55aa55aa55aaULL;
constexpr Bitboard black_squares_bb = ~white_squares_bb;

constexpr Bitboard rank1_bb = 0x00000000000000ffULL;
constexpr Bitboard rank2_bb = rank1_bb << (8 * 1);
constexpr Bitboard rank3_bb = rank1_bb << (8 * 2);
constexpr Bitboard rank4_bb = rank1_bb << (8 * 3);
constexpr Bitboard rank5_bb = rank1_bb << (8 * 4);
constexpr Bitboard rank6_bb = rank1_bb << (8 * 5);
constexpr Bitboard rank7_bb = rank1_bb << (8 * 6);
constexpr Bitboard rank8_bb = rank1_bb << (8 * 7);
const Bitboard RANKS_BB[RANK_NUM] = {
    rank1_bb, rank2_bb, rank3_bb, rank4_bb, rank5_bb, rank6_bb, rank7_bb, rank8_bb
};

constexpr Bitboard middle_ranks = rank2_bb | rank3_bb | rank4_bb | rank5_bb | rank6_bb | rank7_bb;

constexpr Bitboard fileA_bb = 0x0101010101010101ULL;
constexpr Bitboard fileB_bb = fileA_bb << 1;
constexpr Bitboard fileC_bb = fileA_bb << 2;
constexpr Bitboard fileD_bb = fileA_bb << 3;
constexpr Bitboard fileE_bb = fileA_bb << 4;
constexpr Bitboard fileF_bb = fileA_bb << 5;
constexpr Bitboard fileG_bb = fileA_bb << 6;
constexpr Bitboard fileH_bb = fileA_bb << 7;
const Bitboard FILES_BB[FILE_NUM] = {
    fileA_bb, fileB_bb, fileC_bb, fileD_bb, fileE_bb, fileF_bb, fileG_bb, fileH_bb
};

const Bitboard NEIGHBOUR_FILES_BB[FILE_NUM] = {
    fileB_bb,
    fileA_bb | fileC_bb,
    fileB_bb | fileD_bb,
    fileC_bb | fileE_bb,
    fileD_bb | fileF_bb,
    fileE_bb | fileG_bb,
    fileF_bb | fileH_bb,
    fileG_bb
};

inline Bitboard forward_ranks_bb(Color side, Square square)
{
    return side == WHITE ? ~rank1_bb << (rank(square) - RANK_1)
                         : ~rank8_bb >> (RANK_8 - rank(square));
}

inline Bitboard passed_pawn_bb(Color side, Square square)
{
    return forward_ranks_bb(side, square) & (NEIGHBOUR_FILES_BB[file(square)] | FILES_BB[file(square)]);
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
    SOUTHEAST = SOUTH + EAST
};

template <Direction dir> Bitboard shift(Bitboard bb)
{
    return dir == NORTH         ? bb << 8 :
           dir == EAST          ? (bb & ~fileH_bb) << 1 :
           dir == SOUTH         ? bb >> 8 :
           dir == WEST          ? (bb & ~fileA_bb) >> 1 :
           dir == NORTHEAST     ? (bb & ~fileH_bb) << 9 :
           dir == NORTHWEST     ? (bb & ~fileA_bb) << 7 :
           dir == SOUTHEAST     ? (bb & ~fileH_bb) >> 7 :
           dir == SOUTHWEST     ? (bb & ~fileA_bb) >> 9 :
           dir == NORTH + NORTH ? bb << 16 :
           dir == SOUTH + SOUTH ? bb >> 16 :
           0ULL;
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
    }
    assert(false);
    return 0ULL;
}

}

#endif  // CHESS_ENGINE_BITBOARD_H_