#ifndef CHESS_ENGINE_POSITION_H_
#define CHESS_ENGINE_POSITION_H_

#include "bitboard.h"
#include "move_bitboards.h"
#include "types.h"
#include "zobrist_hash.h"

#include <bits/stdint-uintn.h>
#include <cassert>
#include <cinttypes>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

namespace engine
{
class Position
{
  public:
    static const std::string STARTPOS_FEN;
    static const std::regex SAN_REGEX;

    explicit Position();
    explicit Position(std::string fen);

    bool operator==(const Position& other) const;

    // generate fen string for position
    std::string fen() const;

    MoveInfo do_move(Move move);
    void undo_move(Move move, MoveInfo moveinfo);

    MoveInfo do_null_move();
    void undo_null_move(MoveInfo moveinfo);

    bool is_in_check(Color side) const;
    bool is_checkmate() const;

    Color color() const { return _current_side; }

    uint32_t half_moves() const { return _half_move_counter; }
    uint32_t ply_count() const { return _ply_counter; }

    bool is_draw() const;
    bool threefold_repetition() const;
    bool rule50() const;
    bool enough_material() const;

    Piece piece_at(Square square) const { return _board[square]; }
    int number_of_pieces(Piece piece) const { return _piece_count[piece]; }
    Square piece_position(Piece piece, int pos) const
    {
        return _piece_position[piece][pos];
    }

    Castling castling_rights() const { return _castling_rights; }
    Square enpassant_square() const { return _enpassant_square; }

    uint64_t hash() const { return _zobrist_hash.get_key(); }
    uint64_t pawn_hash() const { return _zobrist_hash.get_pawnkey(); }

    Bitboard pieces() const;
    Bitboard pieces(Color c) const;
    Bitboard pieces(PieceKind p) const;
    Bitboard pieces(Color c, PieceKind p) const;
    Bitboard pieces(Piece p) const;
    Bitboard pieces(Piece p1, Piece p2) const;
    Bitboard pieces(Color c, PieceKind p1, PieceKind p2) const;

    /*
     * Return number of non-pawn pieces for given side.
     */
    int no_nonpawns(Color c) const;

    Move parse_uci(const std::string& str);
    std::string uci(Move move) const;

    std::string san(Move move) const;

  private:
    void add_piece(Piece piece, Square square);
    void remove_piece(Square square);
    void move_piece(Square from, Square to);

    void change_current_side();
    void set_enpassant_square(Square sq);

    std::string san_without_check(Move move) const;

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
    uint64_t _history[MAX_PLIES];
};

std::ostream& operator<<(std::ostream& stream, const Position& position);

}  // namespace engine

#endif  // CHESS_ENGINE_POSITION_H_
