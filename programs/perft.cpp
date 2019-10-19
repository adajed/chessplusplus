#include <chrono>
#include <iostream>
#include <vector>

#include "movegen.h"
#include "position.h"

using namespace engine;

uint64_t perftBatched(Position& position, int depth)
{
    Move* begin = MOVE_LIST[depth];
    Move* end = generate_moves(position, position.side_to_move(), begin);

    if (depth == 1)
        return end - begin;

    uint64_t sum = 0;
    for (Move* it = begin; it != end; ++it)
    {
        Move move = *it;
        MoveInfo moveinfo = position.do_move(move);

        uint64_t nodes = perft(position, depth - 1);

        position.undo_move(move, moveinfo);

        sum += nodes;
        print_move(std::cout, move);
        std::cout << ": " << nodes << std::endl;
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

    Position position(fen);
    std::cout << position << std::endl << std::endl;

    auto start = std::chrono::steady_clock::now();
    uint64_t score = perftBatched(position, depth);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;


    std::cout << std::endl;
    std::cout << "Total time (ms)  : " << static_cast<int>(elapsed.count() * 1000) << std::endl;
    std::cout << "Nodes searched   : " << score << std::endl;
    std::cout << "Nodes / second   : " << static_cast<int>(score / elapsed.count()) << std::endl;

    return 0;
}
