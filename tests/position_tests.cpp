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

TEST(PositionTest, constructor)
{
    std::vector<std::string> fens = {
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
        "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 0 1",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1",
    };

    for (std::string fen : fens)
    {
        Position position(fen);
        validate(position);

        EXPECT_FALSE(position.is_checkmate());
    }
}

TEST(PositionTest, fen)
{
    std::vector<std::string> fens = {
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
        "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 0 1",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
        "rnbqkb1r/pppppppp/5n2/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 1 2",
    };

    for (std::string fen : fens)
    {
        Position position(fen);
        EXPECT_EQ(position.fen(), fen);
    }
}

TEST(PositionTest, do_move)
{
    using TestCase = std::tuple<std::string, Move, MoveInfo, std::string>;

    std::vector<TestCase> test_cases = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
         create_move(SQ_E2, SQ_E4),
         create_moveinfo(NO_PIECE_KIND, ALL_CASTLING, NO_SQUARE, false, 0),
         "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
        },
        {"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
         create_move(SQ_G8, SQ_F6),
         create_moveinfo(NO_PIECE_KIND, ALL_CASTLING, SQ_E3, false, 0),
         "rnbqkb1r/pppppppp/5n2/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 1 2",
        },
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
         create_move(SQ_E5, SQ_G6),
         create_moveinfo(PAWN, ALL_CASTLING, NO_SQUARE, false, 0),
         "r3k2r/p1ppqpb1/bn2pnN1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1"
        },
    };

    for (const TestCase& test_case : test_cases)
    {
        Position position(std::get<0>(test_case));
        MoveInfo moveinfo = position.do_move(std::get<1>(test_case));
        EXPECT_EQ(moveinfo, std::get<2>(test_case));
        EXPECT_EQ(position.fen(), std::get<3>(test_case));
    }
}

}
