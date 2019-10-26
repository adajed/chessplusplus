#include <iostream>

#include "score.h"
#include "search.h"
#include "weights.h"

using namespace engine;

int main(int argc, char** argv)
{
    init_move_bitboards();

    std::string weights_path(argv[1]);
    std::string fen(argv[2]);
    Position position(fen);

    Weights weights = load(weights_path);
    PositionScorer scorer(weights);
    Search search(scorer);
    search.set_thinking_time(5000LL);

    int move_number = 0;
    while (!position.is_checkmate() && move_number < 400)
    {
        move_number++;
        std::cout << "move number " << move_number << std::endl;
        std::cout << position;

        Move move = search.select_move(position, 6);

        std::cout << "move = " << move_to_string(move) << std::endl;

        position.do_move(move);
    }
    std::cout << position;

    return 0;
}
