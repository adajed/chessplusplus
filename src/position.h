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

class Position
{
    public:
        explicit Position();
        explicit Position(std::string fen);

        bool operator== (const Position& other) const;

        std::string fen() const;

        MoveInfo do_move(Move move);
        void undo_move(Move move, MoveInfo moveinfo);

        bool is_in_check(Color side) const;
        bool is_checkmate() const;

        Color side_to_move() const { return _current_side; }

        uint32_t half_moves() const { return _half_move_counter; }
        uint32_t ply_count() const { return _ply_counter; }

        bool threefold_repetition() const;
        bool rule50() const;

        Piece piece_at(Square square) const { return _board[square]; }
        int number_of_pieces(Piece piece) const { return _piece_count[piece]; }
        Square piece_position(Piece piece, int pos) const { return _piece_position[piece][pos]; }

        Castling castling_rights() const { return _castling_rights; }
        Square enpassant_square() const { return _enpassant_square; }
        GamePhase game_phase() const;
        HashKey hash() const { return _zobrist_hash; }

        Bitboard pieces() const;
        Bitboard pieces(Color c) const;
        Bitboard pieces(PieceKind p) const;
        Bitboard pieces(Color c, PieceKind p) const;

        std::string move_to_string(Move move) const;

    private:

        void add_piece(Piece piece, Square square);
        void remove_piece(Square square);
        void move_piece(Square from, Square to);

        void change_current_side();

        Color _current_side;

        uint8_t _half_move_counter;
        int32_t _ply_counter;

        Piece _board[SQUARE_NUM];
        Square _piece_position[PIECE_NUM][10];
        int _piece_count[PIECE_NUM];

        Bitboard _by_piece_kind_bb[PIECE_KIND_NUM];
        Bitboard _by_color_bb[COLOR_NUM];

        Castling _castling_rights;
        Square _enpassant_square;

        HashKey _zobrist_hash;

        int32_t _history_counter;
        HashKey _history[MAX_PLIES];
};

std::ostream& operator<< (std::ostream& stream, const Position& position);

}

#endif  // CHESS_ENGINE_POSITION_H_
