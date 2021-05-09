#ifndef CHESS_ENGINE_MOVEGEN_H_
#define CHESS_ENGINE_MOVEGEN_H_

#include "bitboard.h"
#include "bithacks.h"
#include "move_bitboards.h"
#include "position.h"
#include "types.h"

#include <cassert>

namespace engine
{
extern Move MOVE_LIST[2 * MAX_DEPTH][MAX_MOVES];
extern Move QUIESCENCE_MOVE_LIST[2 * MAX_DEPTH][MAX_MOVES];

Move* generate_moves(const Position& position, Color side, Move* list);

Move* generate_quiescence_moves(const Position& position, Color side,
                                Move* list);

Bitboard attacked_squares(const Position& position, Color side);

uint64_t perft(Position& position, int depth);

bool is_move_legal(const Position& position, Move move);

}  // namespace engine

#endif  // CHESS_ENGINE_MOVEGEN_H_
