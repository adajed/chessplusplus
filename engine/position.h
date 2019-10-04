#ifndef CHESS_ENGINE_POSITION_H_
#define CHESS_ENGINE_POSITION_H_

#include <cassert>
#include <cinttypes>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "engine.h"
#include "bitboard.h"
#include "types.h"
#include "move_bitboards.h"

namespace engine
{

Bitboard pieces_bb(const Position& position);

Bitboard pieces_bb(const Position& position, Color c);

Bitboard pieces_bb(const Position& position, PieceKind p);

Bitboard pieces_bb(const Position& position, Color c, PieceKind p);

}

#endif  // CHESS_ENGINE_POSITION_H_
