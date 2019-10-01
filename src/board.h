#ifndef CHESS_ENGINE_SRC_BOARD_H_
#define CHESS_ENGINE_SRC_BOARD_H_

#include <cstddef>
#include <cinttypes>
#include <vector>

namespace engine {

using Position = int8_t;
using Piece = int8_t;

const Piece EMPTY = 0;
const Piece PAWN_A_WHITE = 1;
const Piece PAWN_B_WHITE = 2;
const Piece PAWN_C_WHITE = 3;
const Piece PAWN_D_WHITE = 4;
const Piece PAWN_E_WHITE = 5;
const Piece PAWN_F_WHITE = 6;
const Piece PAWN_G_WHITE = 7;
const Piece PAWN_H_WHITE = 8;
const Piece KNIGHT_1_WHITE = 9;
const Piece KNIGHT_2_WHITE = 10;
const Piece BISHOP_1_WHITE = 11;
const Piece BISHOP_2_WHITE = 12;
const Piece ROOK_1_WHITE = 13;
const Piece ROOK_2_WHITE = 14;
const Piece QUEEN_WHITE = 15;
const Piece KING_WHITE = 16;
const Piece PAWN_A_BLACK = 17;
const Piece PAWN_B_BLACK = 18;
const Piece PAWN_C_BLACK = 19;
const Piece PAWN_D_BLACK = 20;
const Piece PAWN_E_BLACK = 21;
const Piece PAWN_F_BLACK = 22;
const Piece PAWN_G_BLACK = 23;
const Piece PAWN_H_BLACK = 24;
const Piece KNIGHT_1_BLACK = 25;
const Piece KNIGHT_2_BLACK = 26;
const Piece BISHOP_1_BLACK = 27;
const Piece BISHOP_2_BLACK = 28;
const Piece ROOK_1_BLACK = 29;
const Piece ROOK_2_BLACK = 30;
const Piece QUEEN_BLACK = 31;
const Piece KING_BLACK = 32;

const Position NO_POSITION = -1;


enum class Player : uint8_t {
    kWHITE = 0,
    kBLACK = 1,
};

// assumes piece != EMPTY
Player get_player(Piece piece)
{
    return piece < 17 ? Player::kWHITE : Player::kBLACK;
}

struct Move
{
    Position source;
    Position destination;
    Piece piece;
};

class Board {
    public:
        static const size_t SIZE = 8;

        Board()
        {
            pieces[index(0, 0)] = ROOK_1_WHITE;
            pieces[index(0, 1)] = KNIGHT_1_WHITE;
            pieces[index(0, 2)] = BISHOP_1_WHITE;
            pieces[index(0, 3)] = QUEEN_WHITE;
            pieces[index(0, 4)] = KING_WHITE;
            pieces[index(0, 5)] = BISHOP_2_WHITE;
            pieces[index(0, 6)] = KNIGHT_2_WHITE;
            pieces[index(0, 7)] = ROOK_2_WHITE;

            pieces[index(1, 0)] = PAWN_A_WHITE;
            pieces[index(1, 1)] = PAWN_B_WHITE;
            pieces[index(1, 2)] = PAWN_C_WHITE;
            pieces[index(1, 3)] = PAWN_D_WHITE;
            pieces[index(1, 4)] = PAWN_E_WHITE;
            pieces[index(1, 5)] = PAWN_F_WHITE;
            pieces[index(1, 6)] = PAWN_G_WHITE;
            pieces[index(1, 7)] = PAWN_H_WHITE;

            pieces[index(6, 0)] = PAWN_A_BLACK;
            pieces[index(6, 1)] = PAWN_B_BLACK;
            pieces[index(6, 2)] = PAWN_C_BLACK;
            pieces[index(6, 3)] = PAWN_D_BLACK;
            pieces[index(6, 4)] = PAWN_E_BLACK;
            pieces[index(6, 5)] = PAWN_F_BLACK;
            pieces[index(6, 6)] = PAWN_G_BLACK;
            pieces[index(6, 7)] = PAWN_H_BLACK;

            pieces[index(7, 0)] = ROOK_1_BLACK;
            pieces[index(7, 1)] = KNIGHT_1_BLACK;
            pieces[index(7, 2)] = BISHOP_1_BLACK;
            pieces[index(7, 3)] = QUEEN_BLACK;
            pieces[index(7, 4)] = KING_BLACK;
            pieces[index(7, 5)] = BISHOP_2_BLACK;
            pieces[index(7, 6)] = KNIGHT_2_BLACK;
            pieces[index(7, 7)] = ROOK_2_BLACK;

            for (int row = 2; row < 6; row++)
                for (int col = 0; col < 8; col++)
                    pieces[index(row, col)] = EMPTY;

            for (Piece piece = PAWN_A_WHITE; piece < PAWN_H_WHITE; ++piece)
                positions[piece] = index(1, piece - PAWN_A_WHITE);

            positions[ROOK_1_WHITE] = index(0, 0);
            positions[KNIGHT_1_WHITE] = index(0, 1);
            positions[BISHOP_1_WHITE] = index(0, 2);
            positions[QUEEN_WHITE] = index(0, 3);
            positions[KING_WHITE] = index(0, 4);
            positions[BISHOP_2_WHITE] = index(0, 5);
            positions[KNIGHT_2_WHITE] = index(0, 6);
            positions[ROOK_2_WHITE] = index(0, 7);

            for (Piece piece = PAWN_A_BLACK; piece < PAWN_H_BLACK; ++piece)
                positions[piece] = index(6, piece - PAWN_A_BLACK);

            positions[ROOK_1_BLACK] = index(7, 0);
            positions[KNIGHT_1_BLACK] = index(7, 1);
            positions[BISHOP_1_BLACK] = index(7, 2);
            positions[QUEEN_BLACK] = index(7, 3);
            positions[KING_BLACK] = index(7, 4);
            positions[BISHOP_2_BLACK] = index(7, 5);
            positions[KNIGHT_2_BLACK] = index(7, 6);
            positions[ROOK_2_BLACK] = index(7, 7);

        }

        bool is_check_mate() const
        {
            return false;
        }

        std::vector<Move> get_legal_moves() const
        {
            return {};
        }

        void make_move(Move move)
        {
        }

        void undo_move(Move move)
        {
        }

        float score() const
        {
            float value = 0;

            for (Piece piece = PAWN_A_WHITE; piece  < PAWN_H_WHITE; ++piece)
                value += (positions[piece] == NO_POSITION) ? 0. : 1.;
            for (Piece piece = KNIGHT_1_WHITE; piece < QUEEN_WHITE; ++piece)
                value += (positions[piece] == NO_POSITION) ? 0. : 3.;
            value += (positions[QUEEN_WHITE] == NO_POSITION) ? 0. : 5.;

            for (Piece piece = PAWN_A_BLACK; piece  < PAWN_H_BLACK; ++piece)
                value -= (positions[piece] == NO_POSITION) ? 0. : 1.;
            for (Piece piece = KNIGHT_1_BLACK; piece < QUEEN_BLACK; ++piece)
                value -= (positions[piece] == NO_POSITION) ? 0. : 3.;
            value -= (positions[QUEEN_BLACK] == NO_POSITION) ? 0. : 5.;

            return value;

        }

    private:
        Piece pieces[SIZE * SIZE];
        Position positions[33];
        Player current_player;

        Position index(uint8_t row, uint8_t col)
        {
            return row * SIZE + col;
        }
};
} // namespace engine

#endif  // CHESS_ENGINE_SRC_BOARD_H_
