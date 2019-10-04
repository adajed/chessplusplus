#include "position.h"

namespace engine
{

std::ostream& operator<< (std::ostream& stream, const Position& position)
{
    const std::string piece_to_char = " PNBRQKpnbrqk";

    stream << to_fen(position) << std::endl;
    stream << " +---+---+---+---+---+---+---+---+\n";
    for (int rank = 7; rank >= 0; rank--)
    {
        for (int file = 0; file < 8; ++file)
            stream << " | " << piece_to_char[(uint32_t)position.board[make_square(Rank(rank), File(file))]];
        stream << " |\n +---+---+---+---+---+---+---+---+\n";
    }

    return stream;
}

Position initial_position()
{
    return from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Position from_fen(std::string fen)
{
    Position position;
    position.current_side = WHITE;
    for (int i = 0; i < SQUARE_NUM; ++i)
        position.board[i] = NO_PIECE;
    for (int i = 0; i < PIECE_NUM; ++i)
        position.piece_count[i] = 0;
    for (int i = 0; i < PIECE_KIND_NUM; ++i)
        position.by_piece_kind_bb[i] = 0ULL;
    for (int i = 0; i < COLOR_NUM; ++i)
        position.by_color_bb[i] = 0ULL;
    position.castling_rights = NO_CASTLING;
    position.enpassant = NO_SQUARE;

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
            position.board[square] = piece;
            position.by_color_bb[get_color(piece)] |= square_bb(square);
            position.by_piece_kind_bb[get_piece_kind(piece)] |= square_bb(square);
            position.piece_position[piece][position.piece_count[piece]] = square;
            position.piece_count[piece] += 1;
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

    position.current_side = fen[pos++] == 'w' ? WHITE : BLACK;
    pos++;
    while (fen[pos] != ' ')
    {
        switch (fen[pos])
        {
            case 'K': position.castling_rights = Castling(position.castling_rights | W_OO); break;
            case 'Q': position.castling_rights = Castling(position.castling_rights | W_OOO); break;
            case 'k': position.castling_rights = Castling(position.castling_rights | B_OO); break;
            case 'q': position.castling_rights = Castling(position.castling_rights | B_OOO); break;
        }
        pos++;
    }
    pos++;

    if (fen[pos] != '-')
    {
        File f = File(fen[pos++] - 'a');
        Rank r = Rank(fen[pos++] - '1');
        position.enpassant = make_square(r, f);
    }

    return position;
}

std::string to_fen(const Position& position)
{
    const std::string piece_to_char = " PNBRQKpnbrqk";
    std::string result = "";

    int counter = 0;
    for (int rank = 7; rank >= 0; rank--)
    {
        for (int file = 0; file < 8; file++)
        {
            Piece p = position.board[make_square(Rank(rank), File(file))];
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
    result += (position.current_side == WHITE) ? "w" : "b";
    result += " ";
    if (position.castling_rights != NO_CASTLING)
    {
        if (position.castling_rights & W_OO) result += "K";
        if (position.castling_rights & W_OOO) result += "Q";
        if (position.castling_rights & B_OO) result += "k";
        if (position.castling_rights & B_OOO) result += "q";
    }
    else
        result += "-";
    result += " ";
    if (position.enpassant != NO_SQUARE)
    {
        result += char('a' + file(position.enpassant));
        result += char('1' + rank(position.enpassant));
    }
    else
        result += "-";

    return result;
}

Bitboard pieces_bb(const Position& position)
{
    return position.by_color_bb[WHITE] | position.by_color_bb[BLACK];
}

Bitboard pieces_bb(const Position& position, Color c)
{
    return position.by_color_bb[c];
}

Bitboard pieces_bb(const Position& position, PieceKind p)
{
    return position.by_piece_kind_bb[p];
}

Bitboard pieces_bb(const Position& position, Color c, PieceKind p)
{
    return position.by_color_bb[c] & position.by_piece_kind_bb[p];
}

void add_piece(Position& position, Piece piece, Square square)
{
    assert(position.board[square] == NO_PIECE);

    position.board[square] = piece;
    position.by_color_bb[get_color(piece)] |= square_bb(square);
    position.by_piece_kind_bb[get_piece_kind(piece)] |= square_bb(square);
    position.piece_position[piece][position.piece_count[piece]] = square;
    position.piece_count[piece] += 1;
}

void add_piece(Position& position, Color side, PieceKind piece_kind, Square square)
{
    add_piece(position, make_piece(side, piece_kind), square);
}

void remove_piece(Position& position, Square square)
{
    assert(position.board[square] != NO_PIECE);

    Piece piece = position.board[square];
    position.board[square] = NO_PIECE;
    position.by_color_bb[get_color(piece)] ^= square_bb(square);
    position.by_piece_kind_bb[get_piece_kind(piece)] ^= square_bb(square);

    for (int i = 0; i < position.piece_count[piece] - 1; ++i)
    {
        if (position.piece_position[piece][i] == square)
        {
            int pos = position.piece_count[piece] - 1;
            position.piece_position[piece][i] = position.piece_position[piece][pos];
        }
    }
    position.piece_count[piece] -= 1;
}


void do_move(Position& pos, Move& move)
{
    Color side = pos.current_side;
    pos.current_side = !pos.current_side;

    Piece moved_piece = pos.board[move.from];
    Piece captured_piece = pos.board[move.to];
    move.capture = make_piece_kind(captured_piece);

    move.last_castling = pos.castling_rights;
    move.last_enpassant = pos.enpassant;

    if (move.castling != NO_CASTLING)
    {
        Rank rank = side == WHITE ? RANK_1 : RANK_8;
        if (move.castling & KING_CASTLING)
        {
            remove_piece(pos, make_square(rank, FILE_E));
            add_piece(pos, side, KING, make_square(rank, FILE_G));
            remove_piece(pos, make_square(rank, FILE_H));
            add_piece(pos, side, ROOK, make_square(rank, FILE_F));
        }
        else
        {
            remove_piece(pos, make_square(rank, FILE_E));
            add_piece(pos, side, KING, make_square(rank, FILE_C));
            remove_piece(pos, make_square(rank, FILE_A));
            add_piece(pos, side, ROOK, make_square(rank, FILE_D));
        }

        pos.castling_rights = Castling(pos.castling_rights & ~CASTLING_RIGHTS[side]);
        pos.enpassant = NO_SQUARE;

        return;
    }

    remove_piece(pos, move.from);

    if (move.enpassant)
    {
        Square enpassant_square = Square(move.to + (side == WHITE ? -8 : 8));
        remove_piece(pos, enpassant_square);
    }
    if (captured_piece != NO_PIECE)
        remove_piece(pos, move.to);

    if (move.promotion != NO_PIECE_KIND)
        add_piece(pos, side, move.promotion, move.to);
    else
        add_piece(pos, moved_piece, move.to);

    if (get_piece_kind(moved_piece) == KING)
        pos.castling_rights = Castling(pos.castling_rights & ~CASTLING_RIGHTS[side]);
    if (get_piece_kind(moved_piece) == ROOK && move.from == KING_SIDE_ROOK_SQUARE[side])
        pos.castling_rights = Castling(pos.castling_rights & ~CASTLING_RIGHTS[side] & ~KING_CASTLING);
    if (get_piece_kind(moved_piece) == ROOK && move.from == QUEEN_SIDE_ROOK_SQUARE[side])
        pos.castling_rights = Castling(pos.castling_rights & ~CASTLING_RIGHTS[side] & ~QUEEN_CASTLING);

    Rank enpassant_rank = (side == WHITE) ? RANK_4 : RANK_5;
    Rank rank2 = (side == WHITE) ? RANK_2 : RANK_7;
    if (get_piece_kind(moved_piece) == PAWN && rank(move.from) == rank2 && rank(move.to) == enpassant_rank)
        pos.enpassant = Square(move.to + (side == WHITE ? -8 : 8));
    else
        pos.enpassant = NO_SQUARE;
}

void undo_move(Position& pos, Move& move)
{
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
            remove_piece(pos, make_square(rank, FILE_G));
            add_piece(pos, side, KING, make_square(rank, FILE_E));
            remove_piece(pos, make_square(rank, FILE_F));
            add_piece(pos, side, ROOK, make_square(rank, FILE_H));
        }
        else
        {
            remove_piece(pos, make_square(rank, FILE_C));
            add_piece(pos, side, KING, make_square(rank, FILE_E));
            remove_piece(pos, make_square(rank, FILE_D));
            add_piece(pos, side, ROOK, make_square(rank, FILE_A));
        }

        return;
    }

    remove_piece(pos, move.to);
    add_piece(pos, moved_piece, move.from);

    if (move.enpassant)
    {
        Square enpassant_square = Square(move.to + (side == WHITE ? -8 : 8));
        add_piece(pos, !side, PAWN, enpassant_square);
    }
    if (captured_piece != NO_PIECE)
        add_piece(pos, captured_piece, move.to);
}

}
