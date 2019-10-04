#ifndef CHESS_ENGINE_POSITION_H_
#define CHESS_ENGINE_POSITION_H_

#include <cassert>
#include <cinttypes>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "bitboard.h"
#include "types.h"
#include "move_bitboards.h"

namespace engine
{

class Position
{

    public:
        Position() : Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
        {
        }

        Position(std::string fen) : move_history()
        {
            for (int i = 0; i < SQUARE_NUM; ++i)
                board[i] = NO_PIECE;

            for (int i = 0; i < PIECE_NUM; ++i)
                piece_count[i] = 0;

            for (int i = 0; i < COLOR_NUM; ++i)
                by_color_bb[i] = 0ULL;

            for (int i = 0; i < PIECE_KIND_NUM; ++i)
                by_piece_kind_bb[i] = 0ULL;

            std::map<char, Piece> char_to_piece = {
                {'P', W_PAWN}, {'N', W_KNIGHT}, {'B', W_BISHOP}, {'R', W_ROOK}, {'Q', W_QUEEN}, {'K', W_KING},
                {'p', B_PAWN}, {'n', B_KNIGHT}, {'b', B_BISHOP}, {'r', B_ROOK}, {'q', B_QUEEN}, {'k', B_KING},
            };

            int pos = 0;
            int rank = RANK_8;
            int file = FILE_A;
            while (rank >= 0)
            {
                if (fen[pos] == '/')
                {
                    pos++;
                    continue;
                }

                char c = fen[pos];
                if ('0' <= c && c <= '9')
                    file += (c - '0');
                else
                {
                    Square square = make_square(Rank(rank), File(file));
                    Piece piece = char_to_piece[c];
                    board[square] = piece;
                    by_color_bb[get_color(piece)] |= square_bb(square);
                    by_piece_kind_bb[get_piece_kind(piece)] |= square_bb(square);
                    piece_position[piece][piece_count[piece]] = square;
                    piece_count[piece] += 1;
                    file += 1;
                }
                pos++;

                if (file > FILE_H)
                {
                    rank -= 1;
                    file = FILE_A;
                }
            }
            pos++;

            current_side = fen[pos++] == 'w' ? WHITE : BLACK;
            pos++;
            castling_rights = NO_CASTLING;
            while (fen[pos] != ' ')
            {
                switch (fen[pos])
                {
                case 'K': castling_rights = Castling(castling_rights | W_OO); break;
                case 'Q': castling_rights = Castling(castling_rights | W_OOO); break;
                case 'k': castling_rights = Castling(castling_rights | B_OO); break;
                case 'q': castling_rights = Castling(castling_rights | B_OOO); break;
                }
                pos++;
            }
            pos++;

            if (fen[pos] != '-')
            {
                File f = File(fen[pos++] - 'a');
                Rank r = Rank(fen[pos++] - '1');
                enpassant = make_square(r, f);
            }
            else
                enpassant = NO_SQUARE;

        }

        std::string to_fen() const
        {
            const std::string piece_to_char = " PNBRQKpnbrqk";
            std::string result = "";

            int counter = 0;
            for (int rank = 7; rank >= 0; rank--)
            {
                for (int file = 0; file < 8; file++)
                {
                    Piece p = board[make_square(Rank(rank), File(file))];
                    if (p == NO_PIECE)
                        counter++;
                    else
                    {
                        if (counter > 0)
                        {
                            result += char('0' + counter);
                            counter = 0;
                        }
                        result += piece_to_char[uint32_t(p)];
                    }
                }
                if (counter > 0)
                {
                    result += char('0' + counter);
                    counter = 0;
                }
                if (rank > 0)
                    result += "/";
            }
            result += " ";
            result += (current_side == WHITE) ? "w" : "b";
            result += " ";
            if (castling_rights != NO_CASTLING)
            {
                if (castling_rights & W_OO) result += "K";
                if (castling_rights & W_OOO) result += "Q";
                if (castling_rights & B_OO) result += "k";
                if (castling_rights & B_OOO) result += "q";
            }
            else
                result += "-";
            result += " ";
            if (enpassant != NO_SQUARE)
            {
                result += char('a' + file(enpassant));
                result += char('1' + rank(enpassant));
            }
            else
                result += "-";

            return result;
        }

        Bitboard pieces() const {
            return by_color_bb[WHITE] | by_color_bb[BLACK];
        }

        Bitboard pieces(Color c) const
        {
            return by_color_bb[c];
        }

        Bitboard pieces(PieceKind p) const
        {
            return by_piece_kind_bb[p];
        }

        Bitboard pieces(Color c, PieceKind p) const
        {
            return by_color_bb[c] & by_piece_kind_bb[p];
        }

        void add_piece(Color side, PieceKind piece_kind, Square square)
        {
            add_piece(make_piece(side, piece_kind), square);
        }

        void add_piece(Piece piece, Square square)
        {
            assert(board[square] == NO_PIECE);

            board[square] = piece;
            by_color_bb[get_color(piece)] |= square_bb(square);
            by_piece_kind_bb[get_piece_kind(piece)] |= square_bb(square);
            piece_position[piece][piece_count[piece]] = square;
            piece_count[piece] += 1;
        }

        void remove_piece(Square square)
        {
            assert(board[square] != NO_PIECE);

            Piece piece = board[square];
            board[square] = NO_PIECE;
            by_color_bb[get_color(piece)] ^= square_bb(square);
            by_piece_kind_bb[get_piece_kind(piece)] ^= square_bb(square);

            for (int i = 0; i < piece_count[piece] - 1; ++i)
            {
                if (piece_position[piece][i] == square)
                {
                    int last_pos = piece_count[piece] - 1;
                    piece_position[piece][i] = piece_position[piece][last_pos];
                }
            }
            piece_count[piece] -= 1;
        }


        Color current_side;
        Piece board[SQUARE_NUM];
        Square piece_position[PIECE_NUM][16];
        int piece_count[PIECE_NUM];

        Bitboard by_piece_kind_bb[PIECE_KIND_NUM];
        Bitboard by_color_bb[COLOR_NUM];

        Castling castling_rights;
        Square enpassant;
        std::vector<Move> move_history;
};

inline std::ostream& operator<< (std::ostream& stream, const Position& position)
{
    const std::string piece_to_char = " PNBRQKpnbrqk";

    stream << position.to_fen() << std::endl;
    stream << " +---+---+---+---+---+---+---+---+\n";
    for (int rank = 7; rank >= 0; rank--)
    {
        for (int file = 0; file < 8; ++file)
            stream << " | " << piece_to_char[(uint32_t)position.board[make_square(Rank(rank), File(file))]];
        stream << " |\n +---+---+---+---+---+---+---+---+\n";
    }

    return stream;
}

inline void do_move(Position& pos, Move& move)
{
    Color side = pos.current_side;
    pos.current_side = !pos.current_side;

    assert(pos.piece_count[make_piece(side, KING)] == 1 && "START");
    assert(pos.piece_count[make_piece(!side, KING)] == 1  && "START");

    move.last_castling = pos.castling_rights;
    move.last_enpassant = pos.enpassant;

    if (move.castling != NO_CASTLING)
    {
        Rank rank = side == WHITE ? RANK_1 : RANK_8;
        if (move.castling & KING_CASTLING)
        {
            pos.remove_piece(make_square(rank, FILE_E));
            pos.add_piece(side, KING, make_square(rank, FILE_G));
            pos.remove_piece(make_square(rank, FILE_H));
            pos.add_piece(side, ROOK, make_square(rank, FILE_F));
        }
        else
        {
            pos.remove_piece(make_square(rank, FILE_E));
            pos.add_piece(side, KING, make_square(rank, FILE_C));
            pos.remove_piece(make_square(rank, FILE_A));
            pos.add_piece(side, ROOK, make_square(rank, FILE_D));
        }

        pos.castling_rights = Castling(pos.castling_rights & ~CASTLING_RIGHTS[side]);
        pos.enpassant = NO_SQUARE;

        pos.move_history.push_back(move);
        return;
    }

    Piece moved_piece = pos.board[move.from];
    Piece captured_piece = pos.board[move.to];
    if (captured_piece != NO_PIECE)
    {
        if (get_piece_kind(captured_piece) == KING)
        {
            std::cout << pos << std::endl;

            std::cout << "MOVES:" << std::endl;
            for (Move& m : pos.move_history)
                std::cout << m << " capture=" << m.capture << " promotion=" << m.promotion << " captured_piece=" << captured_piece << std::endl;
            std::cout << std::endl;

        }
        assert(get_piece_kind(captured_piece) != KING);

        if (get_color(captured_piece) == side)
        {
            pos.move_history.push_back(move);
            std::cout << pos << std::endl;

            std::cout << "MOVES:" << std::endl;
            for (Move& m : pos.move_history)
                std::cout << m << " capture=" << m.capture << " promotion=" << m.promotion << " captured_piece=" << captured_piece << std::endl;

        }
        assert(get_color(captured_piece) != side);
    }
    move.capture = make_piece_kind(captured_piece);

    pos.remove_piece(move.from);

    if (move.enpassant)
    {
        Square enpassant_square = Square(move.to + (side == WHITE ? -8 : 8));
        pos.remove_piece(enpassant_square);
    }
    if (captured_piece != NO_PIECE)
        pos.remove_piece(move.to);

    if (move.promotion != NO_PIECE_KIND)
        pos.add_piece(side, move.promotion, move.to);
    else
        pos.add_piece(moved_piece, move.to);

    if (get_piece_kind(moved_piece) == KING)
        pos.castling_rights = Castling(pos.castling_rights & ~CASTLING_RIGHTS[side]);
    if (get_piece_kind(moved_piece) == ROOK && move.from == KING_SIDE_ROOK_SQUARE[side])
        pos.castling_rights = Castling(pos.castling_rights & ~CASTLING_RIGHTS[side] & ~KING_CASTLING);
    if (get_piece_kind(moved_piece) == ROOK && move.from == QUEEN_SIDE_ROOK_SQUARE[side])
        pos.castling_rights = Castling(pos.castling_rights & ~CASTLING_RIGHTS[side] & ~QUEEN_CASTLING);
    if (make_piece_kind(captured_piece) == ROOK && move.to == KING_SIDE_ROOK_SQUARE[!side])
        pos.castling_rights = Castling(pos.castling_rights & ~CASTLING_RIGHTS[!side] & ~KING_CASTLING);
    if (make_piece_kind(captured_piece) == ROOK && move.to == QUEEN_SIDE_ROOK_SQUARE[!side])
        pos.castling_rights = Castling(pos.castling_rights & ~CASTLING_RIGHTS[!side] & ~QUEEN_CASTLING);

    Rank enpassant_rank = (side == WHITE) ? RANK_4 : RANK_5;
    Rank rank2 = (side == WHITE) ? RANK_2 : RANK_7;
    if (get_piece_kind(moved_piece) == PAWN && rank(move.from) == rank2 && rank(move.to) == enpassant_rank)
        pos.enpassant = Square(move.to + (side == WHITE ? -8 : 8));
    else
        pos.enpassant = NO_SQUARE;

    assert(pos.piece_count[make_piece(side, KING)] == 1 && "END");
    assert(pos.piece_count[make_piece(!side, KING)] == 1 && "END");
    pos.move_history.push_back(move);
}

inline void undo_move(Position& pos, Move& move)
{
    pos.move_history.pop_back();
    pos.current_side = !pos.current_side;
    Color side = pos.current_side;

    Piece moved_piece = pos.board[move.to];
    if (move.promotion != NO_PIECE_KIND)
        moved_piece = make_piece(side, PAWN);
    Piece captured_piece = make_piece(!side, move.capture);

    pos.castling_rights = move.last_castling;
    pos.enpassant = move.last_enpassant;

    if (move.castling != NO_CASTLING)
    {
        Rank rank = side == WHITE ? RANK_1 : RANK_8;
        if (move.castling & KING_CASTLING)
        {
            pos.remove_piece(make_square(rank, FILE_G));
            pos.add_piece(side, KING, make_square(rank, FILE_E));
            pos.remove_piece(make_square(rank, FILE_F));
            pos.add_piece(side, ROOK, make_square(rank, FILE_H));
        }
        else
        {
            pos.remove_piece(make_square(rank, FILE_C));
            pos.add_piece(side, KING, make_square(rank, FILE_E));
            pos.remove_piece(make_square(rank, FILE_D));
            pos.add_piece(side, ROOK, make_square(rank, FILE_A));
        }

        return;
    }

    pos.remove_piece(move.to);
    pos.add_piece(moved_piece, move.from);

    if (move.enpassant)
    {
        Square enpassant_square = Square(move.to + (side == WHITE ? -8 : 8));
        pos.add_piece(!side, PAWN, enpassant_square);
    }
    if (captured_piece != NO_PIECE)
        pos.add_piece(captured_piece, move.to);
}

}

#endif  // CHESS_ENGINE_POSITION_H_
