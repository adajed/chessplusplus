#include <chrono>
#include <iostream>
#include <vector>

#include "engine.h"
#include "movegen.h"

using namespace engine;

uint64_t perft(Position& position, int depth)
{
    Move* begin = MOVE_LIST[depth];
    Move* end = generate_moves(position, begin);

    if (depth == 1)
        return end - begin;

    uint64_t sum = 0;
    for (Move* it = begin; it != end; ++it)
    {
        Move move = *it;
        MoveInfo moveinfo = do_move(position, move);

        sum += perft(position, depth - 1);

        undo_move(position, move, moveinfo);
    }

    return sum;
}

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
    uint64_t score = perft(position, depth);
    auto end = std::chrono::steady_clock::now();

    uint64_t time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    uint64_t speed = (unsigned long long)score * 1000000ULL / time;

    std::cout << "[depth " << depth << "] score=" << score << ", time=" << double(time) * 1e-6  << "s, speed = " << speed << "nps" << std::endl;

    return 0;
}
