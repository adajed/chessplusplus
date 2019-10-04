#include <iostream>

#include "engine.h"

using namespace engine;

int main()
{
    init_move_bitboards();

    Position position = initial_position();

    for (int i = 0; i < 20; ++i)
    {
        std::cout << position;
        ScoredMove move = minimax(position, 5);

        std::cout << move.move << std::endl;

        do_move(position, move.move);
    }
    std::cout << position;

    return 0;
}
