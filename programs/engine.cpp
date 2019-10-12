#include <iostream>

#include "engine.h"
#include "score.h"
#include "search.h"
#include "weights.h"

using namespace engine;

int main(int argc, char** argv)
{
    init_move_bitboards();

    std::string weights_path(argv[1]);
    std::string fen(argv[2]);
    Position position = from_fen(fen);

    Weights weights(weights_path);
    PositionScorer scorer(weights);
    Search search(scorer);
    search.set_thinking_time(120ULL * 1000000ULL);

    for (int i = 0; i < 60; ++i)
    {
        std::cout << position;

        Move move = search.select_move(position, 6);

        std::cout << "move = ";
        print_move(move);
        std::cout << std::endl;

        do_move(position, move);
    }
    std::cout << position;

    return 0;
}
