#ifndef CHESS_ENGINE_MOVE_BITBOARDS_H_
#define CHESS_ENGINE_MOVE_BITBOARDS_H_

#include <cassert>

#include "bitboard.h"
#include "types.h"

namespace engine
{

enum RayDirection : uint32_t
{
    RAY_NW = 0, RAY_N = 1, RAY_NE = 2, RAY_E = 3, RAY_SE = 4, RAY_S = 5, RAY_SW = 6, RAY_W = 7
};
constexpr uint32_t RAYS_NUM = 8;

constexpr RayDirection opposite_ray(RayDirection ray)
{
    return RayDirection((ray + 4) & 7);
}

constexpr bool allowed_ray(PieceKind piece, RayDirection ray)
{
    assert(piece == BISHOP || piece == ROOK || piece == QUEEN);
    return piece == BISHOP ? !(ray & 1) :
           piece == ROOK   ? ray & 1 :
           true;
}

ENABLE_INCREMENT_OPERATIONS(RayDirection)

extern Bitboard RAYS[RAYS_NUM][SQUARE_NUM];

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

}

#endif  // CHESS_ENGINE_MOVE_BITBOARDS_H_
