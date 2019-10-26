#include "movegen.h"
#include "uci.h"
#include "zobrist_hash.h"

using namespace engine;

int main(int argc, char** argv)
{
    init_move_bitboards();
    zobrist::init();

    Uci uci;
    uci.loop();

    return 0;
}
