#ifndef CHESS_ENGINE_HASH_H_
#define CHESS_ENGINE_HASH_H_

#include <cinttypes>

#include "engine.h"

namespace engine
{

HashKey hash_position(const Position& position);

void update_hash(Position& position, Move move);

/*
void update_hash_after_undo_move(Position& position, Move move,
                                 MoveInfo moveinfo);
                                 */

}  // namespace engine

#endif  // CHESS_ENGINE_HASH_H_
