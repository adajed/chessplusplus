#include <gtest/gtest.h>

#include "movegen.h"
#include "position.h"

using namespace engine;

namespace
{

using TestCase = std::pair<std::string, std::vector<uint64_t>>;

TEST(Perft, perft)
{
    std::vector<TestCase> test_cases = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
         {20, 400, 8902, 197281, 4865609, 119060324}},

        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ",
         {48, 2039, 97862, 4085603, 193690690}},

        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ",
         {14, 191, 2812, 43238, 674624, 11030083, 178633661}},

        {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - ",
         {6, 264, 9467, 422333, 15833292, 706045033}},

        {"r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1",
         {6, 264, 9467, 422333, 15833292, 706045033}},

        {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - ",
         {44, 1486, 62379, 2103487, 89941194}},

        {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - ",
         {46, 2079, 89890, 3894594, 164075551}},
    };

    for (const TestCase& test_case : test_cases)
    {
        std::string fen = test_case.first;
        Position position(fen);
        std::vector<uint64_t> nodes = test_case.second;
        for (int i = 0; i < nodes.size(); ++i)
        {
            int depth = i + 1;
            uint64_t score = perft(position, depth);
            EXPECT_EQ(score, nodes[i]) << "fen=\"" << fen << "\", depth=" << depth;
        }
    }
}

}
