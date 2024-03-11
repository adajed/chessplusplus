#include <gtest/gtest.h>

#include "syzygy/tbprobe.h"
#include "utils.h"

using namespace engine;

namespace
{

/*
namespace TB = Tablebases;

TEST(syzygy, probe_wdl)
{
    using TestCase = std::tuple<std::string, TB::ProbeState, TB::WDLScore>;

    const std::vector<TestCase> testCases = {
        {"8/5kb1/8/8/8/4N3/2K1R3/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"4K3/6R1/8/8/3N4/8/k7/b7 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"4k3/6r1/8/3P4/8/2R5/8/4K3 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"1k6/6r1/3P4/8/8/3R4/8/4K3 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"1k6/6r1/3P1K2/8/8/3R4/8/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/8/8/8/4k2K/1p6/3B1P2/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/6r1/3q4/3B4/8/4k3/8/K7 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"8/k7/4pb2/8/8/8/3Kn3/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/k7/6Q1/5r2/8/8/2P4K/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/n7/8/5K2/5p2/8/2k1P3/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/8/5k1K/2b5/2r5/8/1N6/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"3K4/k7/1N6/2Q5/4n3/8/8/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/3K4/b7/2k5/3P4/8/n7/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"8/6r1/8/1k6/7R/1rK5/8/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"7K/8/8/8/8/3N4/3k2n1/6b1 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"2b5/7Q/8/5k2/8/4B3/6K1/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/8/7N/6k1/1B6/3Q4/5K2/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"6k1/8/1K4P1/8/8/8/6Q1/2Q5 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"4n1k1/8/8/8/8/P7/n1K5/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/3q2K1/8/7Q/1B1k4/8/8/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/8/3r4/8/4r2k/8/Q6K/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/8/8/8/4Kn2/8/1q6/k6Q b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/6Rn/5r2/8/K7/8/4k3/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/3k4/3r4/8/7p/8/5P2/7K b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"3r4/6k1/8/3Q4/8/1K5n/8/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"8/r2k4/8/8/3K4/5r2/8/7R b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"8/3Q4/8/8/6kN/8/K6p/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/8/8/1Q4K1/8/1q6/5kP1/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"8/6K1/8/8/8/3Q4/1r4N1/1k6 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/4b3/5P2/8/7K/3n4/5k2/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"8/4N3/2N5/8/7R/6k1/8/3K4 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"8/3B4/2n5/3n4/k7/8/8/3K4 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/7k/B3q3/8/8/8/5KP1/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/4q3/K7/6pb/8/8/7k/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/7Q/1P6/4k1K1/2R5/8/8/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"8/8/k1K5/8/8/1n1B4/2B5/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"6k1/7R/8/2B5/4K3/8/8/3R4 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"4R3/5k2/8/b2K4/8/8/2b5/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"2k5/7p/6K1/8/8/3r2R1/8/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"3K4/7k/1Q6/2q5/7r/8/8/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"3K3n/k7/3B4/8/8/8/8/3r4 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/8/3N1K2/1k1r4/8/8/8/1n6 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"2K5/8/8/b1k4Q/5b2/8/8/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"5b2/8/8/3p4/k2n4/7K/8/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/8/6k1/6n1/5K2/6p1/8/1B6 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"5r2/8/3K4/r7/8/7k/8/6r1 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"8/8/8/1q6/2Bq4/8/4k3/7K b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"1N6/4n3/8/8/8/4k3/1K6/N7 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"2k5/8/8/1n2K3/5P2/8/1B6/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"k2B4/8/8/4K1q1/8/8/7N/8 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"k6K/7Q/8/8/8/8/2B5/5Q2 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"6Q1/7r/8/3K4/8/7k/3r4/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/4b3/1KR5/8/8/4b3/5k2/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"2k5/8/R7/Q7/8/8/3B4/4K3 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"3k4/4p3/8/8/Q7/8/8/K5R1 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"8/8/8/2q1r3/2k5/r7/8/6K1 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"7n/8/8/2r2p2/8/3k4/7K/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"2K5/1r6/8/8/r7/1nk5/8/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"6b1/8/8/2K1B1R1/8/8/8/k7 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"2Q5/r7/1rk1K3/8/8/8/8/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"r7/8/2R5/8/8/k7/4N3/4K3 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"6R1/b7/q1k5/8/8/8/2K5/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"8/8/1r6/5B2/8/8/8/3K1k1N w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/4k3/8/8/7K/5RNb/8/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"3q3k/8/8/8/5RK1/2q5/8/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"2k5/1R6/8/K5q1/1B6/8/8/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"7K/8/5P2/4k3/4qq2/8/8/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"8/5k2/1K6/8/1p6/B5N1/8/8 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"5Q2/8/3B4/k4Q2/8/5K2/8/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/2k5/6r1/8/4K3/8/3p4/1N6 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/Q7/7K/1k6/5r2/8/5p2/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"3q4/8/1K6/6Q1/4k3/8/P7/8 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"2K5/5Q2/7k/8/8/3N4/4R3/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"R7/8/7k/8/3q2B1/8/8/4K3 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/2p5/5K2/3b4/Q7/8/8/7k b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/8/2p5/8/3K1k2/2Q5/8/n7 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"8/8/8/K7/8/Q5k1/6N1/3N4 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"2Q5/8/8/6k1/8/8/8/3n1K1q w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"k5K1/8/1p6/8/8/8/1RR5/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"1k6/4Q2K/3n4/8/5p2/8/8/8 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"8/6k1/K6R/R7/8/3p4/8/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"8/7k/7p/8/8/p7/3Kp3/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"8/4q1Q1/2K2Q2/8/8/8/7k/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"5B2/8/5K2/8/3B4/3q4/5k2/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"8/6Q1/7K/3p4/4b3/8/6k1/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"N7/6N1/8/2P5/8/8/2k5/6K1 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"8/8/1B6/k4n2/8/1K6/7n/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/k7/5R2/8/5r2/K7/1N6/8 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"2K5/3P2R1/P7/8/8/8/1k6/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"8/2pk3K/8/7n/8/8/2R5/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"7B/8/8/8/1K6/3p2k1/1N6/8 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"3b4/1P6/8/4K3/8/8/1k1P4/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"1n2k3/8/1K6/8/8/3r4/8/7q b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"8/8/2Q5/k3K3/8/3p4/2R5/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/B7/2R5/7b/8/K7/2k5/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/8/6K1/7Q/3R4/7p/8/2k5 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, TB::WDLScore::WDLWin},
        {"8/K7/8/8/8/3kbN1q/8/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"kR6/8/8/4P3/5bK1/8/8/8 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"k2B4/n7/7K/4r3/8/8/8/8 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"5n2/8/7q/k7/8/1K6/8/6N1 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLWin},
        {"8/2r2p2/4K3/8/8/8/8/3k2n1 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/8/R6K/8/4k3/8/4p3/3n4 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/1N6/6k1/8/8/8/q1K5/2R5 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
        {"8/8/4bP2/K7/8/8/R7/2k5 b - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLDraw},
        {"8/8/3q1p2/8/8/1K4b1/8/5k2 w - - 0 1", TB::ProbeState::OK, TB::WDLScore::WDLLoss},
    };

    TB::init(SYZYGY_TEST_DIR);

    for (const TestCase& testCase : testCases)
    {
        Position position{std::get<0>(testCase)};
        TB::ProbeState err;
        TB::WDLScore wdl = TB::probe_wdl(position, &err);
        EXPECT_EQ(err, std::get<1>(testCase)) << std::get<0>(testCase);
        EXPECT_EQ(wdl, std::get<2>(testCase)) << std::get<0>(testCase);
    }
}

TEST(syzygy, probe_dtz)
{
    using TestCase = std::tuple<std::string, TB::ProbeState, int>;

    const std::vector<TestCase> testCases = {
        {"8/5kb1/8/8/8/4N3/2K1R3/8 w - - 0 1", TB::ProbeState::OK, 25},
        {"4K3/6R1/8/8/3N4/8/k7/b7 w - - 0 1", TB::ProbeState::OK, 21},
        {"1k6/6r1/3P1K2/8/8/3R4/8/8 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"2k5/8/8/5N2/1N6/8/K7/4r3 b - - 0 1", TB::ProbeState::OK, 0},
        {"4n3/2k5/8/P6K/8/8/8/4b3 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"7k/8/7r/2R5/8/7K/2b5/8 w - - 0 1", TB::ProbeState::OK, 0},
        {"Br6/6K1/4B3/8/8/5k2/8/8 b - - 0 1", TB::ProbeState::OK, 0},
        {"K7/8/8/8/6q1/P7/N7/4k3 w - - 0 1", TB::ProbeState::OK, -6},
        {"qb6/8/7R/8/8/3k4/8/4K3 b - - 0 1", TB::ProbeState::OK, 3},
        {"1N6/1K6/8/1P6/3r4/8/7k/8 w - - 0 1", TB::ProbeState::OK, 0},
        {"5q2/2N5/8/7Q/8/8/k2K4/8 w - - 0 1", TB::ProbeState::OK, 0},
        {"8/8/1K2B3/4N3/4k3/8/2N5/8 w - - 0 1", TB::ProbeState::OK, 13},
        {"8/8/8/6k1/8/K2B1Q2/3n4/8 b - - 0 1", TB::ProbeState::OK, 0},
        {"8/8/8/1q6/2b5/8/2q5/3k1K2 w - - 0 1", TB::ProbeState::OK, -2},
        {"8/5K2/RB6/8/7k/4P3/8/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -2},
        {"3K3k/8/8/7N/8/8/3Q4/2r5 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"7n/8/5k2/4r3/3K4/8/8/6Q1 w - - 0 1", TB::ProbeState::OK, 0},
        {"8/8/6b1/4P3/k7/8/1R6/6K1 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"2K5/8/Qr6/8/8/1b1k4/8/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"8/K7/8/8/7q/2k5/6N1/4r3 b - - 0 1", TB::ProbeState::OK, 3},
        {"5k2/N7/QK6/8/8/6n1/8/8 w - - 0 1", TB::ProbeState::OK, 3},
        {"5Q2/8/5K2/1k6/b6Q/8/8/8 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"6B1/8/n7/4k1K1/2Q5/8/8/8 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"8/b1Q5/4B3/4K3/8/8/7k/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -10},
        {"B4B2/8/4k3/P7/8/8/2K5/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -2},
        {"8/8/8/2K5/6QB/1k6/5n2/8 b - - 0 1", TB::ProbeState::OK, 0},
        {"4n3/4Q3/2n5/8/8/8/4k3/6K1 b - - 0 1", TB::ProbeState::OK, 0},
        {"3b3K/8/8/2B5/r7/1k6/8/8 b - - 0 1", TB::ProbeState::OK, 17},
        {"8/1r6/8/1N6/6bK/8/8/k7 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -24},
        {"8/8/6B1/1k6/6b1/8/8/1K5B b - - 0 1", TB::ProbeState::OK, 0},
        {"8/3K4/7N/5B1k/8/8/7r/8 b - - 0 1", TB::ProbeState::OK, 0},
        {"2k1r3/5N2/7n/8/8/3K4/8/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"2kr4/5Q2/8/8/8/p7/8/7K w - - 0 1", TB::ProbeState::OK, 5},
        {"8/8/8/6R1/4k3/K7/8/N1Q5 w - - 0 1", TB::ProbeState::OK, 1},
        {"2R5/K7/4Q3/8/5k1Q/8/8/8 b - - 0 1", TB::ProbeState::OK, -2},
        {"8/5k2/8/8/2p3n1/8/8/2B1K3 b - - 0 1", TB::ProbeState::OK, 0},
        {"1K6/3P4/8/8/7p/8/8/5qk1 w - - 0 1", TB::ProbeState::OK, 0},
        {"8/8/n7/5n2/5Q2/1k6/8/7K w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"4k3/8/8/6R1/4B1N1/7K/8/8 b - - 0 1", TB::ProbeState::OK, -8},
        {"5B2/8/8/8/k2K3P/8/7b/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -4},
        {"3kb2K/8/Q7/8/8/8/4B3/8 b - - 0 1", TB::ProbeState::OK, -8},
        {"8/8/8/1R6/4k3/2b5/8/2b2K2 w - - 0 1", TB::ProbeState::OK, 0},
        {"7n/B7/2K5/8/5k1N/8/8/8 w - - 0 1", TB::ProbeState::OK, 0},
        {"2n5/8/7p/8/3k4/8/8/1K5N w - - 0 1", TB::ProbeState::OK, 0},
        {"8/4K3/8/3PP3/8/8/8/1kr5 b - - 0 1", TB::ProbeState::OK, -3},
        {"2B5/3b4/7k/8/7R/8/2K5/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -26},
        {"8/8/8/3q4/8/6B1/1k6/4rK2 w - - 0 1", TB::ProbeState::OK, -4},
        {"8/k6r/5K2/8/8/7r/2n5/8 w - - 0 1", TB::ProbeState::OK, -6},
        {"8/np6/p7/2K5/8/3k4/8/8 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -2},
        {"1K6/4k3/8/8/8/8/3R1p2/3N4 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"8/5N1k/8/3r2p1/8/5K2/8/8 w - - 0 1", TB::ProbeState::OK, -6},
        {"2b4k/8/8/8/1p5K/8/5B2/8 b - - 0 1", TB::ProbeState::OK, 0},
        {"B7/k6P/2K5/8/8/1R6/8/8 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"5K2/2q5/k7/5n2/8/6b1/8/8 w - - 0 1", TB::ProbeState::OK, -2},
        {"Q2k4/8/8/5N2/2q5/8/8/3K4 b - - 0 1", TB::ProbeState::OK, 0},
        {"5K2/3k1p2/6q1/8/8/8/8/4r3 b - - 0 1", TB::ProbeState::OK, 1},
        {"8/3p3b/2n5/8/8/5K2/8/5k2 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"1k1B4/2p5/7P/8/4K3/8/8/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -2},
        {"2K1bb2/8/4B3/8/2k5/8/8/8 b - - 0 1", TB::ProbeState::OK, 0},
        {"8/8/8/6k1/8/8/K5r1/2b2r2 w - - 0 1", TB::ProbeState::OK, -6},
        {"3n4/6N1/2p5/8/8/4k1K1/8/8 b - - 0 1", TB::ProbeState::OK, 0},
        {"1N6/2k4K/8/8/1N6/8/8/3R4 w - - 0 1", TB::ProbeState::OK, 7},
        {"6b1/1r6/n7/1k6/8/8/6K1/8 w - - 0 1", TB::ProbeState::OK, -12},
        {"2k5/8/5K2/6p1/8/6q1/2b5/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"8/8/7p/7K/8/2n4b/6k1/8 w - - 0 1", TB::ProbeState::OK, -2},
        {"6q1/1K4k1/8/8/8/6NR/8/8 b - - 0 1", TB::ProbeState::OK, 7},
        {"1r6/8/k4N2/8/8/7K/8/6B1 b - - 0 1", TB::ProbeState::OK, 0},
        {"6Nk/1K2rn2/8/8/8/8/8/8 w - - 0 1", TB::ProbeState::OK, 0},
        {"8/8/8/3p1r2/4k3/8/8/3KN3 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"2K4k/8/6N1/2B5/8/8/6Q1/8 b - - 0 1", TB::ProbeState::OK, -4},
        {"8/6K1/8/6Q1/7r/8/5k2/4n3 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"K7/8/4p3/8/2b3B1/8/7k/8 w - - 0 1", TB::ProbeState::OK, 0},
        {"8/8/4N3/3q4/2K5/5N2/8/7k w - - 0 1", TB::ProbeState::OK, 0},
        {"8/4B3/8/2K5/8/1B6/4k3/7q w - - 0 1", TB::ProbeState::OK, -116},
        {"6n1/2N5/1k6/8/7N/2K5/8/8 b - - 0 1", TB::ProbeState::OK, 0},
        {"7Q/6K1/1n6/2R5/3k4/8/8/8 w - - 0 1", TB::ProbeState::OK, 3},
        {"8/6N1/8/7K/5k2/8/3PR3/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -2},
        {"n7/8/3p4/8/4k3/K7/8/1r6 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"8/1R6/8/8/5k2/8/1K1N1r2/8 b - - 0 1", TB::ProbeState::OK, 0},
        {"r7/4k3/8/8/3b4/3K4/3n4/8 b - - 0 1", TB::ProbeState::OK, 7},
        {"8/8/8/4K1B1/n1p5/8/8/3k4 w - - 0 1", TB::ProbeState::OK, 0},
        {"7K/1k6/8/8/6P1/8/1n4B1/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -2},
        {"4N3/1K3k2/6b1/8/8/6b1/8/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"7N/8/4p3/8/8/1k2B3/8/1K6 w - - 0 1", TB::ProbeState::OK, 5},
        {"8/6N1/2b5/2K5/8/8/7k/3N4 w - - 0 1", TB::ProbeState::OK, 0},
        {"4k3/1P6/6P1/8/8/3n4/8/5K2 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -2},
        {"2r5/8/1k6/7K/8/8/3p4/1B6 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -2},
        {"8/4k3/4N3/8/5n2/8/8/K2Q4 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -6},
        {"8/4N3/7B/8/5P2/8/3K4/5k2 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"8/2k4p/8/8/8/1K6/1p6/7q b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"1B6/2k5/Kn6/2B5/8/8/8/8 b - - 0 1", TB::ProbeState::OK, 0},
        {"1n1N4/8/8/8/8/3k1p2/5K2/8 w - - 0 1", TB::ProbeState::OK, 0},
        {"1R1Q4/8/5K2/7p/2k5/8/8/8 w - - 0 1", TB::ProbeState::OK, 1},
        {"8/8/1p6/6K1/3p4/2N5/8/5k2 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"8/8/8/1n4k1/8/6b1/8/3R2K1 w - - 0 1", TB::ProbeState::OK, 0},
        {"3r4/8/8/8/7K/1Q6/4kQ2/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -1},
        {"k7/8/2p5/8/1K6/p7/3P4/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"1N6/8/8/3R4/3K4/3b3k/8/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -18},
        {"8/5k2/8/8/1p5B/1K6/8/2R5 w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"6K1/1r6/1b2k3/2Q5/8/8/8/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"4q3/8/8/3P4/4K3/8/8/3k3b w - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, -2},
        {"6Nk/8/8/7q/K7/6q1/8/8 b - - 0 1", TB::ProbeState::ZEROING_BEST_MOVE, 1},
        {"8/8/1K6/8/8/2b5/6N1/2N4k w - - 0 1", TB::ProbeState::OK, 0},
    };

    TB::init(SYZYGY_TEST_DIR);

    for (const TestCase& testCase : testCases)
    {
        Position position{std::get<0>(testCase)};
        TB::ProbeState err;
        int dtz = TB::probe_dtz(position, &err);
        EXPECT_EQ(err, std::get<1>(testCase)) << std::get<0>(testCase);
        EXPECT_EQ(dtz, std::get<2>(testCase)) << std::get<0>(testCase);
    }
}
*/

}  // namespace anonymous
