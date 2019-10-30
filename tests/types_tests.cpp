#include <gtest/gtest.h>

#include "types.h"

using namespace engine;

namespace
{

TEST(TypesTest, create_move)
{
    for (Square from_sq = SQ_A1; from_sq <= SQ_H8; ++from_sq)
    {
        for (Square to_sq = SQ_A1; to_sq <= SQ_H8; ++to_sq)
        {
            Move move = create_move(from_sq, to_sq);
            EXPECT_EQ(from(move), from_sq);
            EXPECT_EQ(to(move), to_sq);
            EXPECT_EQ(promotion(move), NO_PIECE_KIND);
            EXPECT_EQ(castling(move), NO_CASTLING);
        }
    }
}

TEST(TypesTest, create_promotion)
{
    for (Square from_sq = SQ_A1; from_sq <= SQ_H8; ++from_sq)
    {
        for (Square to_sq = SQ_A1; to_sq <= SQ_H8; ++to_sq)
        {
            for (PieceKind piecekind = KNIGHT; piecekind <= QUEEN; ++piecekind)
            {
                Move move = create_promotion(from_sq, to_sq, piecekind);
                EXPECT_EQ(from(move), from_sq);
                EXPECT_EQ(to(move), to_sq);
                EXPECT_EQ(promotion(move), piecekind);
                EXPECT_EQ(castling(move), NO_CASTLING);
            }
        }
    }
}

TEST(TypesTest, create_castling)
{
    for (Castling c : {KING_CASTLING, QUEEN_CASTLING})
    {
        Move move = create_castling(c);
        EXPECT_EQ(castling(move), c);
    }
}

TEST(TypesTest, create_moveinfo)
{
    for (uint32_t piecekind = NO_PIECE_KIND; piecekind <= QUEEN; ++piecekind)
    {
        for (uint32_t castling = NO_CASTLING; castling <= ALL_CASTLING; ++castling)
        {
            for (Square square = SQ_A1; square <= NO_SQUARE; ++square)
            {
                for (bool b : {false, true})
                {
                    for (uint8_t half_move = 0; half_move < 100; ++half_move)
                    {
                        MoveInfo moveinfo = create_moveinfo(PieceKind(piecekind), Castling(castling),
                                                            square, b, half_move);
                        EXPECT_EQ(captured_piece(moveinfo), PieceKind(piecekind));
                        EXPECT_EQ(last_castling(moveinfo), Castling(castling));
                        EXPECT_EQ(last_enpassant(moveinfo), square != NO_SQUARE);
                        if (square != NO_SQUARE)
                            EXPECT_EQ(last_enpassant_square(moveinfo), square);
                        EXPECT_EQ(enpassant(moveinfo), b);
                        EXPECT_EQ(half_move_counter(moveinfo), half_move);
                    }
                }
            }
        }
    }
}

}
