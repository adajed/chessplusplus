#include <iostream>

#include "position.h"
#include "movegen.h"
#include "move_bitboards.h"
#include "minimax.h"

using namespace engine;

int main()
{
    init_move_bitboards();

    Position position;

    for (int i = 0; i < 10; ++i)
    {
        std::cout << position;
        ScoredMove move = minimax(position, 5);

        std::cout << move.move << std::endl;

        do_move(position, move.move);
    }
    std::cout << position;

    return 0;
}
