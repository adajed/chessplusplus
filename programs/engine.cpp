#include <iostream>

#include "engine.h"

using namespace engine;

int main(int argc, char** argv)
{
    init_move_bitboards();

    std::string fen(argv[1]);
    Position position = from_fen(fen);

    for (int i = 0; i < 20; ++i)
    {
        std::cout << position;
        ScoredMove move = minimax(position, 6, false);

        std::cout << move.move << std::endl;

        do_move(position, move.move);
    }
    std::cout << position;

    return 0;
}
