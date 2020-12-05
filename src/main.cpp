#include "endgame.h"
#include "movegen.h"
#include "transposition_table.h"
#include "uci.h"
#include "zobrist_hash.h"

using namespace engine;

int main(int argc, char** argv)
{
    init_move_bitboards();
    init_zobrist();
    bitbase::init();

    Uci uci;
    uci.loop();

    return 0;
}
