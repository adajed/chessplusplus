#include "move_bitboards.h"

#include "types.h"

namespace engine
{

Bitboard RAYS[RAYS_NUM][SQUARE_NUM];
Bitboard KNIGHT_MOVES[SQUARE_NUM];
Bitboard KING_MOVES[SQUARE_NUM];
Bitboard CASTLING_PATHS[1 << 4];
Bitboard LINES[SQUARE_NUM][SQUARE_NUM];

Castling CASTLING_RIGHTS[COLOR_NUM] = {W_CASTLING, B_CASTLING};
Square KING_SIDE_ROOK_SQUARE[COLOR_NUM] = {SQ_H1, SQ_H8};
Square QUEEN_SIDE_ROOK_SQUARE[COLOR_NUM] = {SQ_A1, SQ_A8};
/* Castling CASTLING_RIGHTS[COLOR_NUM]; */
/* Square KING_SIDE_ROOK_SQUARE[COLOR_NUM]; */
/* Square QUEEN_SIDE_ROOK_SQUARE[COLOR_NUM]; */


namespace
{

void init_rays_bitboards()
{
    Direction directions[] = {
        NORTHWEST, NORTH, NORTHEAST, EAST,
        SOUTHEAST, SOUTH, SOUTHWEST, WEST
    };

    for (int i = 0; i < 8; ++i)
    {
        Direction dir = directions[i];
        for (Square sq = SQ_A1; sq <= SQ_H8; ++sq)
        {
            Bitboard bb = 0ULL;
            Bitboard field = shift(square_bb(sq), dir);
            while (field)
            {
                bb |= field;
                field = shift(field, dir);
            }
            RAYS[i][sq] = bb;
        }
    }
}

void init_knight_moves_bitboards()
{
    for (Square sq = SQ_A1; sq <= SQ_H8; ++sq)
    {
        Bitboard bb = 0ULL;
        bb |= shift<NORTH>(shift<NORTH>(shift<EAST>(square_bb(sq))));
        bb |= shift<NORTH>(shift<NORTH>(shift<WEST>(square_bb(sq))));
        bb |= shift<SOUTH>(shift<SOUTH>(shift<EAST>(square_bb(sq))));
        bb |= shift<SOUTH>(shift<SOUTH>(shift<WEST>(square_bb(sq))));
        bb |= shift<EAST>(shift<EAST>(shift<NORTH>(square_bb(sq))));
        bb |= shift<EAST>(shift<EAST>(shift<SOUTH>(square_bb(sq))));
        bb |= shift<WEST>(shift<WEST>(shift<NORTH>(square_bb(sq))));
        bb |= shift<WEST>(shift<WEST>(shift<SOUTH>(square_bb(sq))));
        KNIGHT_MOVES[sq] = bb;
    }
}

void init_king_moves_bitboards()
{
    for (Square sq = SQ_A1; sq <= SQ_H8; ++sq)
    {
        Bitboard bb = 0ULL;
        bb |= shift<NORTH>(square_bb(sq));
        bb |= shift<SOUTH>(square_bb(sq));
        bb |= shift<EAST>(square_bb(sq));
        bb |= shift<WEST>(square_bb(sq));
        bb |= shift<NORTHEAST>(square_bb(sq));
        bb |= shift<NORTHWEST>(square_bb(sq));
        bb |= shift<SOUTHEAST>(square_bb(sq));
        bb |= shift<SOUTHWEST>(square_bb(sq));
        KING_MOVES[sq] = bb;
    }
}

void init_castling_paths_bitboards()
{
    CASTLING_PATHS[1] = 0x0000000000000060ULL;
    CASTLING_PATHS[2] = 0x000000000000000EULL;
    CASTLING_PATHS[4] = 0x6000000000000000ULL;
    CASTLING_PATHS[8] = 0x0E00000000000000ULL;
}

void init_lines_bitboards()
{
    Direction directions[] = {
        NORTHWEST, NORTH, NORTHEAST, EAST,
        SOUTHEAST, SOUTH, SOUTHWEST, WEST
    };
    int moves[] = {7, 8, 9, 1, -7, -8, -9, -1};

    for (Square from = SQ_A1; from <= SQ_H8; ++from)
        for (Square to = SQ_A1; to <= SQ_H8; ++to)
            LINES[from][to] = 0ULL;

    for (Square from = SQ_A1; from <= SQ_H8; ++from)
    {
        for (int i = 0; i < 8; ++i)
        {
            Square to = from;
            Bitboard to_bb = square_bb(to);
            Bitboard bb = to_bb;

            while (to_bb)
            {
                LINES[from][to] = bb;
                bb |= shift(bb, directions[i]);
                to_bb = shift(to_bb, directions[i]);
                to = Square(to + moves[i]);
            }
        }
    }
}

}

void init_move_bitboards()
{
    init_rays_bitboards();
    init_knight_moves_bitboards();
    init_king_moves_bitboards();
    init_castling_paths_bitboards();
    init_lines_bitboards();
}

}
