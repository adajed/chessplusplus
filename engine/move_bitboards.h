#ifndef CHESS_ENGINE_MOVE_BITBOARDS_H_
#define CHESS_ENGINE_MOVE_BITBOARDS_H_

#include <cassert>

#include "bitboard.h"
#include "types.h"

namespace engine
{

enum Ray : uint32_t
{
    RAY_NW = 0, RAY_N = 1, RAY_NE = 2, RAY_E = 3, RAY_SE = 4, RAY_S = 5, RAY_SW = 6, RAY_W = 7
};
constexpr uint32_t RAYS_NUM = 8;

constexpr Ray opposite_ray(Ray ray)
{
    return Ray((ray + 4) & 7);
}

constexpr bool allowed_ray(PieceKind piece, Ray ray)
{
    assert(piece == BISHOP || piece == ROOK || piece == QUEEN);
    return piece == BISHOP ? !(ray & 1) :
           piece == ROOK   ? ray & 1 :
           true;
}

ENABLE_INCREMENT_OPERATIONS(Ray)

extern Bitboard RAYS[RAYS_NUM][SQUARE_NUM];

/*
 * Returns squares that given slider piece would attack
 *  on empty board.
 */
template <PieceKind piecekind>
inline Bitboard pseudoattacks(Square sq)
{
    assert(piecekind == BISHOP || piecekind == ROOK || piecekind == QUEEN);
    assert(sq != NO_SQUARE);

    Bitboard bb = 0ULL;
    if (piecekind == BISHOP || piecekind == QUEEN)
    {
        bb |= (RAYS[RAY_NW][sq] | RAYS[RAY_NE][sq] | RAYS[RAY_SE][sq] | RAYS[RAY_SW][sq]);
    }
    if (piecekind == ROOK || piecekind == QUEEN)
    {
        bb |= (RAYS[RAY_N][sq] | RAYS[RAY_S][sq] | RAYS[RAY_W][sq] | RAYS[RAY_E][sq]);
    }

    return bb;
}

// contains all squares that a king can move to
//  from a given sqare
extern Bitboard KING_MASK[SQUARE_NUM];

// contains all squares that a knight can move to
//  from a given sqare
extern Bitboard KNIGHT_MASK[SQUARE_NUM];

extern Bitboard BISHOP_MASK[SQUARE_NUM];

extern Bitboard ROOK_MASK[SQUARE_NUM];

extern Bitboard CASTLING_PATHS[1 << 4];
extern Bitboard QUEEN_CASTLING_BLOCK[COLOR_NUM];

extern Bitboard LINES[SQUARE_NUM][SQUARE_NUM];

extern uint64_t ROOK_MAGICS[SQUARE_NUM];
extern uint64_t BISHOP_MAGICS[SQUARE_NUM];

extern int ROOK_INDEX_BITS[SQUARE_NUM];
extern int BISHOP_INDEX_BITS[SQUARE_NUM];

extern Bitboard ROOK_TABLE[SQUARE_NUM][4096];
extern Bitboard BISHOP_TABLE[SQUARE_NUM][4096];

void init_move_bitboards();

template <PieceKind piece>
inline Bitboard slider_attack(Square sq, Bitboard blockers)
{
    assert(false);
    return 0ULL;
}

template <>
inline Bitboard slider_attack<BISHOP>(Square sq, Bitboard blockers)
{
    blockers &= BISHOP_MASK[sq];
    uint64_t key = (blockers * BISHOP_MAGICS[sq]) >> (64 - BISHOP_INDEX_BITS[sq]);
    return BISHOP_TABLE[sq][key];
}

template <>
inline Bitboard slider_attack<ROOK>(Square sq, Bitboard blockers)
{
    blockers &= ROOK_MASK[sq];
    uint64_t key = (blockers * ROOK_MAGICS[sq]) >> (64 - ROOK_INDEX_BITS[sq]);
    return ROOK_TABLE[sq][key];
}

template <>
inline Bitboard slider_attack<QUEEN>(Square sq, Bitboard blockers)
{
    return slider_attack<BISHOP>(sq, blockers)
            | slider_attack<ROOK>(sq, blockers);
}

}

#endif  // CHESS_ENGINE_MOVE_BITBOARDS_H_
