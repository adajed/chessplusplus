#ifndef CHESS_ENGINE_H_
#define CHESS_ENGINE_H_

#include <cinttypes>
#include <string>

namespace engine
{

using Bitboard = uint64_t;

enum Color : uint32_t
{
    WHITE, BLACK,
};
constexpr uint32_t COLOR_NUM = 2;

enum Square : uint32_t
{
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    NO_SQUARE
};
constexpr uint32_t SQUARE_NUM = 64;

enum PieceKind : uint32_t
{
    NO_PIECE_KIND,
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};
constexpr uint32_t PIECE_KIND_NUM = 7;

enum Piece : uint32_t
{
    NO_PIECE,
    W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
    B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
};
constexpr uint32_t PIECE_NUM = 13;

enum Rank : uint32_t
{
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8,
};
constexpr uint32_t RANK_NUM = 8;
extern Bitboard RANKS_BB[RANK_NUM];

enum File : uint32_t
{
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H,
};
constexpr uint32_t FILE_NUM = 8;
extern Bitboard FILES_BB[FILE_NUM];

enum Castling : uint32_t
{
    NO_CASTLING = 0,
    W_OO  = 1 << 0,
    W_OOO = 1 << 1,
    B_OO  = 1 << 2,
    B_OOO = 1 << 3,
    W_CASTLING = W_OO | W_OOO,
    B_CASTLING = B_OO | B_OOO,
    KING_CASTLING = W_OO | B_OO,
    QUEEN_CASTLING = W_OOO | B_OOO,
    ALL_CASTLING = W_CASTLING | B_CASTLING
};

struct Move
{
    Square from;
    Square to;
    PieceKind capture;
    PieceKind promotion;
    Castling castling;
    Castling last_castling;
    Square last_enpassant;
    bool enpassant;
};

struct ScoredMove
{
    Move move;
    int64_t score;
};

struct Position
{
    Color current_side;
    Piece board[SQUARE_NUM];
    Square piece_position[PIECE_NUM][16];
    int piece_count[PIECE_NUM + 1];

    Bitboard by_piece_kind_bb[PIECE_KIND_NUM];
    Bitboard by_color_bb[COLOR_NUM];

    Castling castling_rights;
    Square enpassant;
};

Position initial_position();

Position from_fen(std::string fen);

std::string to_fen(const Position& position);

void do_move(Position& position, Move& move);

void undo_move(Position& position, Move& move);

int64_t score(const Position& position);

ScoredMove minimax(Position& position, int depth, bool use_alpha_beta_prunning);

std::ostream& operator<< (std::ostream& stream, const Position& position);

std::ostream& operator<< (std::ostream& stream, const Move& move);

void init_move_bitboards();

uint64_t perft(Position& position, int depth, bool print_moves);

bool is_in_check(const Position& position);

}

#endif  // CHESS_ENGINE_H_
