#include <chrono>
#include <iostream>

#include "movegen.h"
#include "minimax.h"

using namespace engine;

uint32_t perft(Position& position, int depth)
{
    Move* begin = MOVE_LIST[depth];
    Move* end = generate_moves(position, begin);

    if (depth == 1)
        return end - begin;


    uint32_t sum = 0;
    for (Move* it = begin; it != end; ++it)
    {
        do_move(position, *it);
        sum += perft(position, depth - 1);
        undo_move(position, *it);
    }

    return sum;
}

int main()
{
    init_move_bitboards();

    Position position;
    std::cout << position;

    for (int depth = 1; depth < 10; ++depth)
    {
        position = Position();
        auto start = std::chrono::steady_clock::now();
        uint32_t score = perft(position, depth);
        auto end = std::chrono::steady_clock::now();

        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        double time_d = double(time) * 1e-6;
        uint64_t speed = uint64_t(double(score) / time_d);

        std::cout << "[depth " << depth << "] score=" << score << ", time=" << time_d << "s, speed=" << speed << " nodes/s" << std::endl;
    }

    return 0;
}
