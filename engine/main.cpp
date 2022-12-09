#include "endgame.h"
#include "movegen.h"
#include "uci.h"
#include "zobrist_hash.h"

using namespace engine;

int main()
{
    init_move_bitboards();
    init_zobrist();
    bitbase::init();
    endgame::init();

    Uci uci;
    uci.loop();

    return 0;
}
