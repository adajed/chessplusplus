#include <gtest/gtest.h>

#include "move_bitboards.h"
#include "zobrist_hash.h"

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);

    engine::init_move_bitboards();
    engine::init_zobrist();

    return RUN_ALL_TESTS();
}
