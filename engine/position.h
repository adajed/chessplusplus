#ifndef CHESS_ENGINE_POSITION_H_
#define CHESS_ENGINE_POSITION_H_

#include <cassert>
#include <cinttypes>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "bitboard.h"
#include "move_bitboards.h"
#include "types.h"

namespace engine
{

struct Position
{
    Color current_side;
    Piece board[SQUARE_NUM];
    Square piece_position[PIECE_NUM][10];
    int piece_count[PIECE_NUM];

    Bitboard by_piece_kind_bb[PIECE_KIND_NUM];
    Bitboard by_color_bb[COLOR_NUM];

    Castling castling_rights;
    Square enpassant;

    HashKey zobrist_hash;
};

bool operator== (const Position& position1, const Position& position2);

Bitboard pieces_bb(const Position& position);

Bitboard pieces_bb(const Position& position, Color c);

Bitboard pieces_bb(const Position& position, PieceKind p);

Bitboard pieces_bb(const Position& position, Color c, PieceKind p);


Position initial_position();

Position from_fen(std::string fen);

std::string to_fen(const Position& position);

MoveInfo do_move(Position& position, Move move);

void undo_move(Position& position, Move move, MoveInfo moveinfo);


bool is_in_check(const Position& position);

bool is_checkmate(const Position& position);

GamePhase get_game_phase(const Position& position);

std::ostream& operator<< (std::ostream& stream, const Position& position);

}

#endif  // CHESS_ENGINE_POSITION_H_
