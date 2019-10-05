#include <gtest/gtest.h>

#include "engine.h"

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    engine::init_move_bitboards();
    return RUN_ALL_TESTS();
}
