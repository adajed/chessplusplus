#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>
#include "types.h"
#include "bitboard.h"
#include "bithacks.h"
#include "position_bitboards.h"
#include "zobrist_hash.h"

#include <functional>
#include <gtest/internal/gtest-param-util.h>
#include <map>
#include <gtest/gtest_pred_impl.h>
#include <vector>

using namespace engine;

// check if only squares that are in bb satisfy pred
// (i.e. pred(x, y) <=> (x, y) in bb)
bool check_bb(Bitboard bb, std::function<bool(int, int)> pred)
{
    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            if (pred(rank, file) != bool(bb & square_bb(make_square(Rank(rank), File(file)))))
                return false;
        }
    }
    return true;
}

Bitboard bb_from_function(std::function<bool(int, int)> pred)
{
    Bitboard bb = 0ULL;
    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            Square sq = make_square(Rank(rank), File(file));
            if (pred(rank, file))
                bb |= square_bb(sq);
        }
    }

    return bb;
}

TEST(Bitboard, forward_ranks_bb)
{
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            Square sq = make_square(Rank(rank), File(file));
            EXPECT_TRUE(check_bb(forward_ranks_bb<WHITE>(sq), [rank](int x, int y){return x > rank;}));
            EXPECT_TRUE(check_bb(forward_ranks_bb<BLACK>(sq), [rank](int x, int y){return x < rank;}));
        }
    }
}

TEST(Bitboard, passed_pawn_bb)
{
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            Square sq = make_square(Rank(rank), File(file));

            auto f_white = [rank, file](int x, int y) {
                return x > rank && abs(y - file) <= 1;
            };
            EXPECT_TRUE(check_bb(passed_pawn_bb<WHITE>(sq), f_white));

            auto f_black = [rank, file](int x, int y) {
                return x < rank && abs(y - file) <= 1;
            };
            EXPECT_TRUE(check_bb(passed_pawn_bb<BLACK>(sq), f_black));
        }
    }
}

TEST(Bitboard, squares_left_behind_bb)
{
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            Square sq = make_square(Rank(rank), File(file));

            auto f_white = [rank, file](int x, int y) {
                return x <= rank && abs(y - file) == 1;
            };
            EXPECT_TRUE(check_bb(squares_left_behind_bb<WHITE>(sq), f_white));

            auto f_black = [rank, file](int x, int y) {
                return x >= rank && abs(y - file) == 1;
            };
            EXPECT_TRUE(check_bb(squares_left_behind_bb<BLACK>(sq), f_black));
        }
    }
}

TEST(Bitboard, get_outposts)
{
    const std::vector<std::string> testCases = {
        "r2q1rk1/ppp2ppp/3p1n2/4p3/1bPnP3/2NP1BPP/PP1B1P2/R2QK2R b KQ - 2 10",
        "6k1/5ppp/8/8/8/8/8/1RK5 w - - 0 1",
        "r5k1/5ppp/8/8/8/8/1R6/1RK5 w - - 0 1",
        "rr4k1/5ppp/8/8/8/2R5/2R5/2RK4 w - - 0 1",
        "1k1r4/pp1b1R2/3q2pp/4p3/2B5/4Q3/PPP2B2/2K5 b - - 0 1",
        "3r1k2/4npp1/1ppr3p/p6P/P2PPPP1/1NR5/5K2/2R5 w - - 0 1",
        "2q1rr1k/3bbnnp/p2p1pp1/2pPp3/PpP1P1P1/1P2BNNP/2BQ1PRK/7R b - - 0 1",
        "rnbqkb1r/p3pppp/1p6/2ppP3/3N4/2P5/PPP1QPPP/R1B1KB1R w KQkq - 0 1",
        "r1b2rk1/2q1b1pp/p2ppn2/1p6/3QP3/1BN1B3/PPP3PP/R4RK1 w - - 0 1",
        "2r3k1/pppR1pp1/4p3/4P1P1/5P2/1P4K1/P1P5/8 w - - 0 1",
        "1nk1r1r1/pp2n1pp/4p3/q2pPp1N/b1pP1P2/B1P2R2/2P1B1PP/R2Q2K1 w - - 0 1",
        "4b3/p3kp2/6p1/3pP2p/2pP1P2/4K1P1/P3N2P/8 w - - 0 1",
        "2kr1bnr/pbpq4/2n1pp2/3p3p/3P1P1B/2N2N1Q/PPP3PP/2KR1B1R w - - 0 1",
        "3rr1k1/pp3pp1/1qn2np1/8/3p4/PP1R1P2/2P1NQPP/R1B3K1 b - - 0 1",
        "2r1nrk1/p2q1ppp/bp1p4/n1pPp3/P1P1P3/2PBB1N1/4QPPP/R4RK1 w - - 0 1",
        "r3r1k1/ppqb1ppp/8/4p1NQ/8/2P5/PP3PPP/R3R1K1 b - - 0 1",
        "r2q1rk1/4bppp/p2p4/2pP4/3pP3/3Q4/PP1B1PPP/R3R1K1 w - - 0 1",
        "2r3k1/1p2q1pp/2b1pr2/p1pp4/6Q1/1P1PP1R1/P1PN2PP/5RK1 w - - 0 1",
        "r1bqkb1r/4npp1/p1p4p/1p1pP1B1/8/1B6/PPPN1PPP/R2Q1RK1 w kq - 0 1",
        "r2q1rk1/1ppnbppp/p2p1nb1/3Pp3/2P1P1P1/2N2N1P/PPB1QP2/R1B2RK1 b - - 0 1",
        "r1bq1rk1/pp2ppbp/2np2p1/2n5/P3PP2/N1P2N2/1PB3PP/R1B1QRK1 b - - 0 1",
        "3rr3/2pq2pk/p2p1pnp/8/2QBPP2/1P6/P5PP/4RRK1 b - - 0 1",
        "r4k2/pb2bp1r/1p1qp2p/3pNp2/3P1P2/2N3P1/PPP1Q2P/2KRR3 w - - 0 1",
        "3rn2k/ppb2rpp/2ppqp2/5N2/2P1P3/1P5Q/PB3PPP/3RR1K1 w - - 0 1",
        "2r2rk1/1bqnbpp1/1p1ppn1p/pP6/N1P1P3/P2B1N1P/1B2QPP1/R2R2K1 b - - 0 1",
        "r1bqk2r/pp2bppp/2p5/3pP3/P2Q1P2/2N1B3/1PP3PP/R4RK1 b kq - 0 1",
        "r2qnrnk/p2b2b1/1p1p2pp/2pPpp2/1PP1P3/PRNBB3/3QNPPP/5RK1 w - - 0 1",
        "rn1qkb1r/pp2pppp/5n2/3p1b2/3P4/2N1P3/PP3PPP/R1BQKBNR w KQkq - 0 1",
        "rn1qkb1r/pp2pppp/5n2/3p1b2/3P4/1QN1P3/PP3PPP/R1B1KBNR b KQkq - 1 1",
        "r1bqk2r/ppp2ppp/2n5/4P3/2Bp2n1/5N1P/PP1N1PP1/R2Q1RK1 b kq - 1 10",
        "r1bqrnk1/pp2bp1p/2p2np1/3p2B1/3P4/2NBPN2/PPQ2PPP/1R3RK1 w - - 1 12",
        "rnbqkb1r/ppp1pppp/5n2/8/3PP3/2N5/PP3PPP/R1BQKBNR b KQkq - 3 5",
        "rnbq1rk1/pppp1ppp/4pn2/8/1bPP4/P1N5/1PQ1PPPP/R1B1KBNR b KQ - 1 5",
        "r4rk1/3nppbp/bq1p1np1/2pP4/8/2N2NPP/PP2PPB1/R1BQR1K1 b - - 1 12",
        "rn1qkb1r/pb1p1ppp/1p2pn2/2p5/2PP4/5NP1/PP2PPBP/RNBQK2R w KQkq c6 1 6",
        "r1bq1rk1/1pp2pbp/p1np1np1/3Pp3/2P1P3/2N1BP2/PP4PP/R1NQKB1R b KQ - 1 9",
        "rnbqr1k1/1p3pbp/p2p1np1/2pP4/4P3/2N5/PP1NBPPP/R1BQ1RK1 w - - 1 11",
        "rnbqkb1r/pppp1ppp/5n2/4p3/4PP2/2N5/PPPP2PP/R1BQKBNR b KQkq f3 1 3",
        "r1bqk1nr/pppnbppp/3p4/8/2BNP3/8/PPP2PPP/RNBQK2R w KQkq - 2 6",
        "rnbq1b1r/ppp2kpp/3p1n2/8/3PP3/8/PPP2PPP/RNBQKB1R b KQ d3 1 5",
        "rnbqkb1r/pppp1ppp/3n4/8/2BQ4/5N2/PPP2PPP/RNB2RK1 b kq - 1 6",
        "r2q1rk1/2p1bppp/p2p1n2/1p2P3/4P1b1/1nP1BN2/PP3PPP/RN1QR1K1 w - - 1 12",
        "r1bqkb1r/2pp1ppp/p1n5/1p2p3/3Pn3/1B3N2/PPP2PPP/RNBQ1RK1 b kq - 2 7",
        "r2qkbnr/2p2pp1/p1pp4/4p2p/4P1b1/5N1P/PPPP1PP1/RNBQ1RK1 w kq - 1 8",
        "1rbq1rk1/p1b1nppp/1p2p3/8/1B1pN3/P2B4/1P3PPP/2RQ1R1K w - - 0 1",
        "3r2k1/p2r1p1p/1p2p1p1/q4n2/3P4/PQ5P/1P1RNPP1/3R2K1 b - - 0 1",
        "r1b1r1k1/1ppn1p1p/3pnqp1/8/p1P1P3/5P2/PbNQNBPP/1R2RB1K w - - 0 1",
        "2r4k/pB4bp/1p4p1/6q1/1P1n4/2N5/P4PPP/2R1Q1K1 b - - 0 1",
        "2r2rk1/1bqnbpp1/1p1ppn1p/pP6/N1P1P3/P2B1N1P/1B2QPP1/R2R2K1 b - - 0 1",
        "5r1k/6pp/1n2Q3/4p3/8/7P/PP4PK/R1B1q3 b - - 0 1",
        "r3k2r/pbn2ppp/8/1P1pP3/P1qP4/5B2/3Q1PPP/R3K2R w KQkq - 0 1",
        "3r2k1/ppq2pp1/4p2p/3n3P/3N2P1/2P5/PP2QP2/K2R4 b - - 0 1",
        "q3rn1k/2QR4/pp2pp2/8/P1P5/1P4N1/6n1/6K1 w - - 0 1",
        "6k1/p3q2p/1nr3pB/8/3Q1P2/6P1/PP5P/3R2K1 b - - 0 1",
        "1r4k1/7p/5np1/3p3n/8/2NB4/7P/3N1RK1 w - - 0 1",
        "1r2r1k1/p4p1p/6pB/q7/8/3Q2P1/PbP2PKP/1R3R2 w - - 0 1",
        "r2q1r1k/pb3p1p/2n1p2Q/5p2/8/3B2N1/PP3PPP/R3R1K1 w - - 0 1",
        "8/4p3/p2p4/2pP4/2P1P3/1P4k1/1P1K4/8 w - - 0 1",
        "1r1q1rk1/p1p2pbp/2pp1np1/6B1/4P3/2NQ4/PPP2PPP/3R1RK1 w - - 0 1",
        "q4rk1/1n1Qbppp/2p5/1p2p3/1P2P3/2P4P/6P1/2B1NRK1 b - - 0 1",
        "r2q1r1k/1b1nN2p/pp3pp1/8/Q7/PP5P/1BP2RPN/7K w - - 0 1",
        "8/5p2/pk2p3/4P2p/2b1pP1P/P3P2B/8/7K w - - 0 1",
        "8/2k5/4p3/1nb2p2/2K5/8/6B1/8 w - - 0 1",
        "1B1b4/7K/1p6/1k6/8/8/8/8 w - - 0 1",
        "rn1q1rk1/1b2bppp/1pn1p3/p2pP3/3P4/P2BBN1P/1P1N1PP1/R2Q1RK1 b - - 0 1",
        "8/p1ppk1p1/2n2p2/8/4B3/2P1KPP1/1P5P/8 w - - 0 1",
        "8/3nk3/3pp3/1B6/8/3PPP2/4K3/8 w - - 0 1",
    };

    for (const auto& fen : testCases)
    {
        Position position(fen);

        auto f_white = [&position](int x, int y) {
            Square sq = make_square(Rank(x), File(y));
            Bitboard sq_bb = square_bb(sq);

            if (position.piece_at(sq) == W_PAWN || position.piece_at(sq) == B_PAWN)
                return false;

            if (position.pieces(WHITE, PAWN) & (shift<SOUTHEAST>(sq_bb) | shift<SOUTHWEST>(sq_bb)))
            {
                Bitboard bb = NEIGHBOUR_FILES_BB[File(y)] & forward_ranks_bb<WHITE>(sq);
                return (position.pieces(BLACK, PAWN) & bb) ? false : true;
            }

            return false;
        };


        EXPECT_TRUE(check_bb(get_outposts<WHITE>(position), f_white));

        auto f_black = [&position](int x, int y) {
            Square sq = make_square(Rank(x), File(y));
            Bitboard sq_bb = square_bb(sq);

            if (position.piece_at(sq) == W_PAWN || position.piece_at(sq) == B_PAWN)
                return false;

            if (position.pieces(BLACK, PAWN) & (shift<NORTHEAST>(sq_bb) | shift<NORTHWEST>(sq_bb)))
            {
                Bitboard bb = NEIGHBOUR_FILES_BB[File(y)] & forward_ranks_bb<BLACK>(sq);
                return (position.pieces(WHITE, PAWN) & bb) ? false : true;
            }

            return false;
        };

        EXPECT_TRUE(check_bb(get_outposts<BLACK>(position), f_black));
    }
}

const std::map<Direction, std::tuple<int, int, std::string>> DIRECTION_MAP = {
    {NORTH,         { 1,  0, "north"}},
    {SOUTH,         {-1,  0, "south"}},
    {EAST,          { 0,  1, "east"}},
    {WEST,          { 0, -1, "west"}},
    {NORTHWEST,     { 1, -1, "northwest"}},
    {NORTHEAST,     { 1,  1, "northeast"}},
    {SOUTHWEST,     {-1, -1, "southwest"}},
    {SOUTHEAST,     {-1,  1, "southeast"}},
    {DOUBLENORTH,   { 2,  0, "doublenorth"}},
    {DOUBLESOUTH,   {-2,  0, "doublesouth"}}
};

const std::vector<Bitboard> testCases = {
    all_squares_bb,
    ~all_squares_bb,
    white_squares_bb,
    black_squares_bb,

    rank1_bb,
    rank2_bb,
    rank3_bb,
    rank4_bb,
    rank5_bb,
    rank6_bb,
    rank7_bb,
    rank8_bb,

    fileA_bb,
    fileB_bb,
    fileC_bb,
    fileD_bb,
    fileE_bb,
    fileF_bb,
    fileG_bb,
    fileH_bb,

    NEIGHBOUR_FILES_BB[0],
    NEIGHBOUR_FILES_BB[1],
    NEIGHBOUR_FILES_BB[2],
    NEIGHBOUR_FILES_BB[3],
    NEIGHBOUR_FILES_BB[4],
    NEIGHBOUR_FILES_BB[5],
    NEIGHBOUR_FILES_BB[6],
    NEIGHBOUR_FILES_BB[7],

    middle_ranks,
};

const std::vector<Direction> ALL_DIRECTIONS = {
    NORTH, SOUTH, EAST, WEST,
    NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST,
    DOUBLENORTH, DOUBLESOUTH
};

class ShiftTest : public testing::TestWithParam<Direction> {};

TEST_P(ShiftTest, shift)
{
    Direction dir = GetParam();

    for (Bitboard bb : testCases)
    {
        auto f = [bb, dir](int x, int y) {
            int px = x - std::get<0>(DIRECTION_MAP.at(dir));
            int py = y - std::get<1>(DIRECTION_MAP.at(dir));

            if (px >= 0 && px < 8 && py >= 0 && py < 8)
                return bool(bb & square_bb(make_square(Rank(px), File(py))));

            return false;
        };
        EXPECT_TRUE(check_bb(shift(bb, dir), f));
    }
}
INSTANTIATE_TEST_SUITE_P(Test, ShiftTest,
                        testing::ValuesIn(ALL_DIRECTIONS),
                        [](const testing::TestParamInfo<ShiftTest::ParamType>& info){
                            return std::get<2>(DIRECTION_MAP.at(info.param));
                        });
