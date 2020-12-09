#include "endgame.h"
#include "movegen.h"
#include "transposition_table.h"
#include "types.h"
#include "uci.h"
#include "zobrist_hash.h"

#include "time_manager.h"

using namespace engine;

int main(int argc, char** argv)
{
    init_move_bitboards();
    init_zobrist();
    bitbase::init();
    endgame::init();

    Uci uci;
    uci.loop();

    return 0;
}
