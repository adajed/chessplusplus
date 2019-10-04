#ifndef CHESS_ENGINE_TYPES_H_
#define CHESS_ENGINE_TYPES_H_

#include <cassert>
#include <cinttypes>
#include <iostream>

/* #include "bitboard.h" */
#include "engine.h"

namespace engine
{

extern Castling CASTLING_RIGHTS[COLOR_NUM];
extern Square KING_SIDE_ROOK_SQUARE[COLOR_NUM];
extern Square QUEEN_SIDE_ROOK_SQUARE[COLOR_NUM];

constexpr Rank rank(Square sq)
{
    return Rank(sq >> 3);
}

constexpr File file(Square sq)
{
    return File(sq & 7);
}

constexpr Square make_square(Rank rank, File file)
{
    return Square((rank << 3) + file);
}

#define ENABLE_BASIC_OPERATIONS(T)                                            \
constexpr T operator+ (T v1, T v2) { return T(uint32_t(v1) + uint32_t(v2)); } \
constexpr T operator- (T v1, T v2) { return T(uint32_t(v1) - uint32_t(v2)); } \
inline T& operator+= (T& v1, T v2) { return v1 = v1 + v2; }                   \
inline T& operator-= (T& v1, T v2) { return v1 = v1 - v2; }                   \
inline T& operator+= (T& v1, uint32_t v2) { return v1 = v1 + T(v2); }         \
inline T& operator-= (T& v1, uint32_t v2) { return v1 = v1 - T(v2); }         \
inline T& operator+= (T& v1, int v2) { return v1 = v1 + T(v2); }              \
inline T& operator-= (T& v1, int v2) { return v1 = v1 - T(v2); }

#define ENABLE_INCREMENT_OPERATIONS(T)                         \
inline T& operator++ (T& v) { return v = T(uint32_t(v) + 1); } \
inline T& operator-- (T& v) { return v = T(uint32_t(v) - 1); }

ENABLE_BASIC_OPERATIONS(Square)
ENABLE_BASIC_OPERATIONS(Rank)
ENABLE_BASIC_OPERATIONS(File)
ENABLE_BASIC_OPERATIONS(Piece)
ENABLE_BASIC_OPERATIONS(PieceKind)

ENABLE_INCREMENT_OPERATIONS(Square)
ENABLE_INCREMENT_OPERATIONS(Rank)
ENABLE_INCREMENT_OPERATIONS(File)

constexpr Bitboard square_bb(Square sq) { return 1ULL << sq; }

constexpr PieceKind make_piece_kind(Piece piece)
{
    if (piece == NO_PIECE)
        return NO_PIECE_KIND;
    return PieceKind((piece - 1) % 6 + 1);
}

constexpr Piece make_piece(Color side, PieceKind piece_kind)
{
    if (piece_kind == NO_PIECE_KIND)
        return NO_PIECE;
    return Piece(piece_kind + 6 * side);
}

constexpr Color operator! (Color color)
{
    return Color(1 - color);
}

constexpr Color get_color(Piece piece)
{
    assert(piece != NO_PIECE);
    return (piece < B_PAWN) ? WHITE : BLACK;
}

constexpr PieceKind get_piece_kind(Piece piece)
{
    assert(piece != NO_PIECE);
    return PieceKind((piece - 1) % 6 + 1);
}

constexpr bool is_piece_slider(Piece piece)
{
    PieceKind piece_kind = get_piece_kind(piece);
    return piece_kind == BISHOP || piece_kind == ROOK || piece_kind == QUEEN;
}

inline void print_bb(Bitboard bb)
{
    std::cout << "##########" << std::endl;
    for (int rank = 7; rank >= 0; rank--)
    {
        std::cout << "#";
        for (int file = 0; file < 8; ++file)
        {
            if (bb & square_bb(make_square(Rank(rank), File(file))))
                std::cout << ".";
            else
                std::cout << " ";
        }
        std::cout << "#" << std::endl;
    }
    std::cout << "##########" << std::endl;
}

}

#endif  // CHESS_ENGINE_TYPES_H_
