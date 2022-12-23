#include "endgame.h"
#include "movegen.h"
#include "uci.h"
#include "zobrist_hash.h"

using namespace engine;

int main()
{
    move_bitboards::init();
    zobrist::init();
    bitbase::init();
    endgame::init();

    Uci uci;
    uci.loop();

    return 0;
}
