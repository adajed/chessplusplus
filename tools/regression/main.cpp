#include "engine_wrapper.h"

#include "endgame.h"
#include "movegen.h"
#include "uci.h"
#include "zobrist_hash.h"

int main(int argc, char** argv)
{
    init_move_bitboards();
    init_zobrist();
    bitbase::init();
    endgame::init();

    EngineWrapper engine1("/home/adam/Projects/chessplusplus/build/chessplusplus");
    EngineWrapper engine2("/home/adam/Projects/shallow-blue/shallowblue");

    engine1.uci();
    engine2.uci();

    Position position(Position::STARTPOS_FEN);
    std::vector<Move> moves;

    CommandGoParams params;
    params.movetime = 5000;

    /* engine1.position(Position::STARTPOS_FEN, moves); */
    /* engine1.go(params); */
    /* engine1.go(params); */
    /* engine1.go(params); */

    uint32_t break_time = 200000;

    while (!position.is_checkmate() && !position.is_draw()) {
        EngineWrapper* e = position.side_to_move() == WHITE ? &engine1 : &engine2;

        e->position(Position::STARTPOS_FEN, moves);
        Move move = e->go(params);
        moves.push_back(move);
        position.do_move(move);

        /* usleep(break_time); */
    }


    engine1.quit();
    engine2.quit();

    return 0;
}
