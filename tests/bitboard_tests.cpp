#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>
#include "types.h"
#include "bitboard.h"
#include "bithacks.h"

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

TEST(Bitboard, forward_ranks_bb)
{
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            Square sq = make_square(Rank(rank), File(file));
            EXPECT_TRUE(check_bb(forward_ranks_bb(WHITE, sq), [rank](int x, int y){return x > rank;}));
            EXPECT_TRUE(check_bb(forward_ranks_bb(BLACK, sq), [rank](int x, int y){return x < rank;}));
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
            EXPECT_TRUE(check_bb(passed_pawn_bb(WHITE, sq), f_white));

            auto f_black = [rank, file](int x, int y) {
                return x < rank && abs(y - file) <= 1;
            };
            EXPECT_TRUE(check_bb(passed_pawn_bb(BLACK, sq), f_black));
        }
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
