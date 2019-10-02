#include <iostream>

#include "position.h"
#include "movegen.h"
#include "move_bitboards.h"
#include "minimax.h"

using namespace engine;

int main()
{
    init_move_bitboards();

    /* Position position = Position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "); */
    /* Position position("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - "); */
    Position position("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -");

    for (int i = 0; i < 1; ++i)
    {
        std::cout << position;
        ScoredMove move = minimax(position, 2);

        std::cout << move.move << std::endl;

        do_move(position, move.move);
    }
    std::cout << position;

    return 0;
}
