#include <gtest/gtest.h>

#include "position.h"

using namespace engine;

namespace
{

void validate(const Position& position)
{
    for (Piece piece = W_PAWN; piece <= B_KING; ++piece)
    {
        Bitboard piece_bb = position.pieces(get_color(piece), get_piece_kind(piece));
        Bitboard bb = 0ULL;

        for (int i = 0; i < position.number_of_pieces(piece); ++i)
        {
            Square square = position.piece_position(piece, i);
            EXPECT_EQ(bb & square_bb(square), 0ULL);
            bb |= square_bb(square);
        }

        EXPECT_EQ(piece_bb, bb);
    }

    for (Piece piece1 = W_PAWN; piece1 <= B_KING; ++piece1)
        for (Piece piece2 = W_PAWN; piece2 <= B_KING; ++piece2)
            if (piece1 != piece2)
            {
                Bitboard bb1 = position.pieces(get_color(piece1), get_piece_kind(piece1));
                Bitboard bb2 = position.pieces(get_color(piece2), get_piece_kind(piece2));
                EXPECT_EQ(bb1 & bb2, 0ULL);
            }

    EXPECT_EQ(position.pieces(WHITE) & position.pieces(BLACK), 0ULL);

    for (Color side : {WHITE, BLACK})
    {
        Bitboard bb1 = position.pieces(side);
        Bitboard bb2 = 0ULL;
        for (PieceKind piecekind : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING})
            bb2 |= position.pieces(side, piecekind);
        EXPECT_EQ(bb1, bb2);
    }

    for (PieceKind piecekind : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING})
    {
        Bitboard bb1 = position.pieces(piecekind);
        Bitboard bb2 = 0ULL;
        for (Color side : {WHITE, BLACK})
            bb2 |= position.pieces(side, piecekind);
        EXPECT_EQ(bb1, bb2);
    }

    for (Piece piece = W_PAWN; piece <= B_KING; ++piece)
    {
        for (int i = 0; i < position.number_of_pieces(piece); ++i)
        {
            Square square = position.piece_position(piece, i);
            EXPECT_EQ(position.piece_at(square), piece);
        }
    }

}

TEST(PositionTest, constructor_startpos)
{
    Position position;

    ASSERT_EQ(position.side_to_move(), WHITE);
    ASSERT_EQ(position.castling_rights(), ALL_CASTLING);
    ASSERT_EQ(position.enpassant_square(), NO_SQUARE);

    EXPECT_EQ(position.pieces(WHITE, PAWN),   0x000000000000FF00ULL);
    EXPECT_EQ(position.pieces(WHITE, KNIGHT), 0x0000000000000042ULL);
    EXPECT_EQ(position.pieces(WHITE, BISHOP), 0x0000000000000024ULL);
    EXPECT_EQ(position.pieces(WHITE, ROOK),   0x0000000000000081ULL);
    EXPECT_EQ(position.pieces(WHITE, QUEEN),  0x0000000000000008ULL);
    EXPECT_EQ(position.pieces(WHITE, KING),   0x0000000000000010ULL);
    EXPECT_EQ(position.pieces(BLACK, PAWN),   0x00FF000000000000ULL);
    EXPECT_EQ(position.pieces(BLACK, KNIGHT), 0x4200000000000000ULL);
    EXPECT_EQ(position.pieces(BLACK, BISHOP), 0x2400000000000000ULL);
    EXPECT_EQ(position.pieces(BLACK, ROOK),   0x8100000000000000ULL);
    EXPECT_EQ(position.pieces(BLACK, QUEEN),  0x0800000000000000ULL);
    EXPECT_EQ(position.pieces(BLACK, KING),   0x1000000000000000ULL);

    validate(position);

    EXPECT_FALSE(position.is_in_check(WHITE));
    EXPECT_FALSE(position.is_in_check(BLACK));
    EXPECT_FALSE(position.is_checkmate());

    EXPECT_EQ(position.half_moves(), 0);
    EXPECT_EQ(position.ply_count(), 1);
}

TEST(PositionTest, pieces)
{
    Position position;
}

}
