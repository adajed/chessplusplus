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
                        EXPECT_EQ(last_enpassant_square(moveinfo), square);
                        EXPECT_EQ(enpassant(moveinfo), b);
                        EXPECT_EQ(half_move_counter(moveinfo), half_move);
                    }
                }
            }
        }
    }
}

TEST(TypesTest, square)
{
    for (Rank r = RANK_1; r <= RANK_8; ++r)
    {
        for (File f = FILE_A; f <= FILE_H; ++f)
        {
            Square sq = make_square(r, f);

            EXPECT_EQ(rank(sq), r);
            EXPECT_EQ(file(sq), f);

            if ((r % 2 == 0 && f % 2 == 1) || (r % 2 == 1 && f % 2 == 0))
                EXPECT_EQ(sq_color(sq), WHITE);
            else
                EXPECT_EQ(sq_color(sq), BLACK);
        }
    }
}

TEST(TypesTest, PieceCountVector)
{
    PieceCountVector pcv = create_pcv(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    EXPECT_EQ(get_count_pcv<W_PAWN>(pcv), 1);
    EXPECT_EQ(get_count_pcv<W_KNIGHT>(pcv), 2);
    EXPECT_EQ(get_count_pcv<W_BISHOP>(pcv), 3);
    EXPECT_EQ(get_count_pcv<W_ROOK>(pcv), 4);
    EXPECT_EQ(get_count_pcv<W_QUEEN>(pcv), 5);
    EXPECT_EQ(get_count_pcv<B_PAWN>(pcv), 6);
    EXPECT_EQ(get_count_pcv<B_KNIGHT>(pcv), 7);
    EXPECT_EQ(get_count_pcv<B_BISHOP>(pcv), 8);
    EXPECT_EQ(get_count_pcv<B_ROOK>(pcv), 9);
    EXPECT_EQ(get_count_pcv<B_QUEEN>(pcv), 10);

    pcv = modify_pcv<W_ROOK>(pcv, 11);
    EXPECT_EQ(get_count_pcv<W_ROOK>(pcv), 11);
    pcv = modify_pcv<B_ROOK>(pcv, 12);
    EXPECT_EQ(get_count_pcv<B_ROOK>(pcv), 12);
}

}
