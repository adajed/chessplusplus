#include <gtest/gtest.h>

#include "endgame.h"
#include "types.h"

using namespace engine;

namespace
{

TEST(bibase, correctness)
{
    //                          strong side, current side, strong king, strong pawn, weak king, result
    using TestCase = std::tuple<      Color,        Color,      Square,      Square,    Square,   bool>;

    const std::vector<TestCase> testCases = {
        {WHITE, WHITE, SQ_B7, SQ_C7, SQ_D7, true},
        {WHITE, BLACK, SQ_F6, SQ_F7, SQ_F8, false},
        {BLACK, WHITE, SQ_E3, SQ_F2, SQ_G2, false},
        {WHITE, BLACK, SQ_D6, SQ_C7, SQ_B7, false},
        {BLACK, WHITE, SQ_F6, SQ_G6, SQ_G3, false},
        {WHITE, WHITE, SQ_C3, SQ_B3, SQ_B6, true},
        {BLACK, BLACK, SQ_F6, SQ_G6, SQ_G3, true},
    };

    for (uint32_t i = 0; i < testCases.size(); ++i)
    {
        TestCase testCase = testCases[i];
        Color side = std::get<1>(testCase);
        Square wKing = std::get<2>(testCase);
        Square wPawn = std::get<3>(testCase);
        Square bKing = std::get<4>(testCase);

        bitbase::normalize(std::get<0>(testCase), side, wKing, wPawn, bKing);

        EXPECT_EQ(bitbase::check(side, wKing, wPawn, bKing), std::get<5>(testCase)) << i;
    }
}

}  // namespace anonymous
