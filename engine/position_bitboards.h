#ifndef CHESS_ENGINE_POSITION_BITBOARD_H_
#define CHESS_ENGINE_POSITION_BITBOARD_H_

#include "bitboard.h"
#include "bithacks.h"
#include "position.h"
#include "types.h"

namespace engine
{

// returns bitboard with outpost squares for given side
// (i.e. squares controlled by side, but no opponent pawn can attack it)
template<Color side>
inline Bitboard get_outposts(const Position& position)
{
    Bitboard opponent_pawns = position.pieces(!side, PAWN);
    Bitboard outpost_bb = 0ULL;

    // fileA
    Bitboard pawn_on_fileB = opponent_pawns & FILES_BB[FILE_B];
    Square sq_fileB;
    if (popcount(pawn_on_fileB))
        sq_fileB = Square(side == WHITE ? msb(pawn_on_fileB) : lsb(pawn_on_fileB));
    else
        sq_fileB = make_square(side == WHITE ? RANK_1 : RANK_8, FILE_B);

    outpost_bb |= (squares_left_behind_bb<!side>(sq_fileB) & FILES_BB[FILE_A]);

    // fileH
    Bitboard pawn_on_fileG = opponent_pawns & FILES_BB[FILE_G];
    Square sq_fileG;
    if (popcount(pawn_on_fileG))
        sq_fileG = Square(side == WHITE ? msb(pawn_on_fileG) : lsb(pawn_on_fileG));
    else
        sq_fileG = make_square(side == WHITE ? RANK_1 : RANK_8, FILE_G);

    outpost_bb |= (squares_left_behind_bb<!side>(sq_fileG) & FILES_BB[FILE_H]);


    for (int i = 1; i < 7; ++i)
    {
        File file1 = File(i - 1);
        File file2 = File(i + 1);

        Bitboard pawn_on_file1_bb = opponent_pawns & FILES_BB[file1];
        Bitboard pawn_on_file2_bb = opponent_pawns & FILES_BB[file2];

        Square sq_file1, sq_file2;

        if (popcount(pawn_on_file1_bb))
            sq_file1 = Square(side == WHITE ? msb(pawn_on_file1_bb) : lsb(pawn_on_file1_bb));
        else
            sq_file1 = make_square(side == WHITE ? RANK_1 : RANK_8, file1);

        if (popcount(pawn_on_file2_bb))
            sq_file2 = Square(side == WHITE ? msb(pawn_on_file2_bb) : lsb(pawn_on_file2_bb));
        else
            sq_file2 = make_square(side == WHITE ? RANK_1 : RANK_8, file2);

        outpost_bb |= (squares_left_behind_bb<!side>(sq_file1) & squares_left_behind_bb<!side>(sq_file2));
    }

    constexpr Direction forward_left = side == WHITE ? NORTHWEST : SOUTHEAST;
    constexpr Direction forward_right = side == WHITE ? NORTHEAST : SOUTHWEST;
    Bitboard pawns = position.pieces(side, PAWN);
    Bitboard attacked_squares = shift<forward_left>(pawns) | shift<forward_right>(pawns);

    outpost_bb &= attacked_squares;
    outpost_bb &= ~position.pieces(PAWN);
    return outpost_bb;
}

template <Color side>
Bitboard backward_pawns(Bitboard ourPawns, Bitboard theirPawns)
{
    constexpr Direction forward  = side == WHITE ? NORTH : SOUTH;
    constexpr Direction backward = side == WHITE ? SOUTH : NORTH;

    Bitboard stops = shift<forward>(ourPawns);
    Bitboard ourAttacks = pawn_attacks<side>(ourPawns);
    Bitboard theirAttacks = pawn_attacks<!side>(theirPawns);

    return shift<backward>(stops & theirAttacks & ~ourAttacks);
}

}  // namespace engine

#endif  // CHESS_ENGINE_POSITION_BITBOARD_H_
