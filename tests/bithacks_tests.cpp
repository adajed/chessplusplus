#include <gtest/gtest.h>
#include "bithacks.h"

using namespace engine;

namespace
{

TEST(Bithacks, lsb)
{
    for (int i = 0; i < 64; ++i)
        EXPECT_EQ(lsb(1ULL << i), i);

    EEXPECT_EQ(lsb(~0ULL), 1);
}

} // namespace anonymous
