#include <gtest/gtest.h>
#include "bithacks.h"

using namespace engine;

namespace
{

TEST(Bithacks, lsb)
{
    EXPECT_EQ(lsb(~0ULL), 0);

    for (int i = 0; i < 64; ++i)
    {
        for (int j = i; j < 64; ++j)
        {
            Bitboard bb = 1ULL << i | 1ULL << j;
            EXPECT_EQ(lsb(bb), i);
        }
    }
}

TEST(Bithacks, msb)
{
    EXPECT_EQ(msb(~0ULL), 63);

    for (int i = 0; i < 64; ++i)
    {
        for (int j = i; j < 64; ++j)
        {
            Bitboard bb = 1ULL << i | 1ULL << j;
            EXPECT_EQ(msb(bb), j);
        }
    }
}

TEST(Bithacks, popcount)
{
    EXPECT_EQ(popcount(~0ULL), 64);
    EXPECT_EQ(popcount(0ULL), 0);

    for (int i = 0; i < 64; ++i)
        EXPECT_EQ(popcount(1ULL << i), 1);
}

TEST(Bithacks, pop_lsb)
{
    Bitboard bb = 0xaf41bc45077818feULL;

    while (bb)
    {
        Bitboard bb_prev = bb;
        uint32_t i = lsb(bb);
        uint32_t j = pop_lsb(&bb);
        EXPECT_EQ(i, j);
        EXPECT_EQ(bb_prev ^ (1ULL << j), bb);
    }
}


} // namespace anonymous
