#ifndef CHESS_ENGINE_TYPES_H_
#define CHESS_ENGINE_TYPES_H_

#include <cassert>
#include <cinttypes>
#include <iostream>

#include "bitboard.h"

namespace engine
{

enum Color : uint32_t
{
    WHITE, BLACK,
};
constexpr uint32_t COLOR_NUM = 2;

enum Square : uint32_t
{
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    NO_SQUARE
};
constexpr uint32_t SQUARE_NUM = 64;

enum PieceKind : uint32_t
{
    NO_PIECE_KIND,
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};
constexpr uint32_t PIECE_KIND_NUM = 7;

enum Piece : uint32_t
{
    NO_PIECE,
    W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
    B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
};
constexpr uint32_t PIECE_NUM = 13;

enum Rank : uint32_t
{
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8,
};
constexpr uint32_t RANK_NUM = 8;

enum File : uint32_t
{
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H,
};
constexpr uint32_t FILE_NUM = 8;

enum Castling : uint32_t
{
    NO_CASTLING = 0,
    W_OO = 1,
    W_OOO = 2,
    B_OO = 4,
    B_OOO = 8,
    W_CASTLING = W_OO | W_OOO,
    B_CASTLING = B_OO | B_OOO, KING_CASTLING = W_OO | B_OO, QUEEN_CASTLING = W_OOO | B_OOO,
    ALL_CASTLING = W_CASTLING | B_CASTLING
};

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

struct Move
{
    Square from;
    Square to;
    PieceKind capture;
    PieceKind promotion;
    Castling castling;
    Castling last_castling;
    Square last_enpassant;
    bool enpassant;
};

inline std::ostream& operator<< (std::ostream& stream, const Move& move)
{
    const std::string files = "abcdefgh";
    const std::string ranks = "12345678";

    stream << files[file(move.from)] << ranks[rank(move.from)] << "-"
           << files[file(move.to)] << ranks[rank(move.to)];

    return stream;

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
