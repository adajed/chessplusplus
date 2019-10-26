#ifndef CHESS_ENGINE_BITHACKS_H_
#define CHESS_ENGINE_BITHACKS_H_

#include "bitboard.h"

namespace engine
{

int lsb(Bitboard bb);

int msb(Bitboard bb);

int popcount(Bitboard bb);

uint32_t pop_lsb(Bitboard* bb);

bool popcount_more_than_one(Bitboard bb);

}

#endif  // CHESS_ENGINE_BITHACKS_H_
