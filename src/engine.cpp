#include "movegen.h"
#include "transposition_table.h"
#include "uci.h"
#include "zobrist_hash.h"

using namespace engine;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " weights_path" << std::endl;
        return 1;
    }


    init_move_bitboards();
    zobrist::init();

    std::string weights_path(argv[1]);
    Weights weights(weights_path);
    PositionScorer scorer(weights);
    Uci uci(scorer);
    uci.loop();

    return 0;
}
