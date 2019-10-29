#include "movegen.h"
#include "uci.h"
#include "zobrist_hash.h"

using namespace engine;

int main(int argc, char** argv)
{
    init_move_bitboards();
    zobrist::init();

    Weights weights = load(WEIGHTSFILE);
    PositionScorer scorer(weights);
    Uci uci(scorer);
    uci.loop();

    return 0;
}