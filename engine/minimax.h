#ifndef CHESS_ENGINE_SRC_MINIMAX_H_
#define CHESS_ENGINE_SRC_MINIMAX_H_

#include "position.h"
#include "movegen.h"

namespace engine {

struct ScoredMove
{
    Move move;
    int32_t score;
};

int32_t score(const Position& position);

ScoredMove minimax(Position& position, int alpha, int beta, int depth, bool verbose);

} // namespace engine

#endif
