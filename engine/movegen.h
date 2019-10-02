#ifndef CHESS_ENGINE_MOVEGEN_H_
#define CHESS_ENGINE_MOVEGEN_H_

#include <cassert>

#include "bitboard.h"
#include "bithacks.h"
#include "move_bitboards.h"
#include "position.h"

namespace engine
{

constexpr int MAX_DEPTH = 40;
constexpr int MAX_MOVES = 256;
constexpr int MAX_PINS = 16;

struct Pin
{
    Square square;
    PieceKind piece_kind;
    RayDirection direction;
};

extern Move MOVE_LIST[MAX_DEPTH][MAX_MOVES];
extern Pin PINS[MAX_PINS];

Move* generate_moves(const Position& position, Move* list);

}

#endif  // CHESS_ENGINE_MOVEGEN_H_
