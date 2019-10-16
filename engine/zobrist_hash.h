#ifndef CHESS_ENGINE_HASH_H_
#define CHESS_ENGINE_HASH_H_

#include "position.h"
#include "types.h"

namespace engine
{

HashKey hash_position(const Position& position);

void update_hash(Position& position, Move move);

void init_zobrist_hash();

}  // namespace engine

#endif  // CHESS_ENGINE_HASH_H_
