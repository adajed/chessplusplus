#include <iostream>
#include <chrono>

#include "position.h"
#include "movegen.h"
#include "move_bitboards.h"
#include "minimax.h"

using namespace engine;

const unsigned long long MOVE_TIME = 15ULL * 1000000ULL;

int main()
{
    init_move_bitboards();

    Position position;

    for (int i = 0; i < 80; ++i)
    {
        std::cout << position;

        auto start_time = std::chrono::steady_clock::now();
        auto end_time = start_time;

        ScoredMove move;

        int depth = 2;
        while (std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() < MOVE_TIME)
        {
            depth += 2;
            move = minimax(position, -(1 << 16), 1 << 16, depth, true);
            end_time = std::chrono::steady_clock::now();
        }

        std::cout << "Finished at depth " << depth << std::endl;
        std::cout << "Time " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << std::endl;
        std::cout << move.move << std::endl;

        do_move(position, move.move);
    }
    std::cout << position;

    return 0;
}
