#ifndef CHESS_ENGINE_HASH_H_
#define CHESS_ENGINE_HASH_H_

#include "position.h"
#include "types.h"

namespace engine
{
namespace zobrist
{

extern uint64_t PIECE_HASH[PIECE_NUM][SQUARE_NUM];
extern uint64_t CASTLING_HASH[1 << 4];
extern uint64_t SIDE_HASH;
extern uint64_t ENPASSANT_HASH[FILE_NUM];

HashKey hash(const Position& position);

void init();

}  // namespace zobrist
}  // namespace engine

#endif  // CHESS_ENGINE_HASH_H_
