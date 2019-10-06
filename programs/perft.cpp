#include <chrono>
#include <iostream>
#include <vector>

#include "engine.h"

using namespace engine;

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " fen_string depth" << std::endl;
        return 1;
    }

    std::string fen = argv[1];
    int depth = atoi(argv[2]);

    init_move_bitboards();

    Position position = from_fen(fen);
    std::cout << position;

    auto start = std::chrono::steady_clock::now();
    uint32_t score = perft(position, depth, true);
    auto end = std::chrono::steady_clock::now();

    uint64_t time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    double time_d = double(time) * 1e-6;
    double speed = double(score) / time_d;

    std::cout << "[depth " << depth << "] score=" << score << ", time=" << time_d << "s, speed = " << speed << std::endl;

    return 0;
}
