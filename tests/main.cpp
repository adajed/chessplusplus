#include <gtest/gtest.h>

#include "endgame.h"
#include "move_bitboards.h"
#include "zobrist_hash.h"

int main(int argc, char** argv)
{
    using namespace engine;

    testing::InitGoogleTest(&argc, argv);

    move_bitboards::init();
    zobrist::init();
    bitbase::init();
    endgame::init();

    return RUN_ALL_TESTS();
}
