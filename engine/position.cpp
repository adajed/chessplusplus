#include "position.h"
#include "movegen.h"
#include "zobrist_hash.h"

namespace engine
{

Square from(Move move)
{
    return Square(move & 0x3F);
}

Square to(Move move)
{
    return Square((move >> 6) & 0x3F);
}

PieceKind promotion(Move move)
{
    return PieceKind((move >> 12) & 0x7);
}

Castling castling(Move move)
{
    int p = (move >> 15) & 0x3;
    return p == 0 ? NO_CASTLING :
           p == 1 ? KING_CASTLING :
           QUEEN_CASTLING;
}

MoveInfo create_moveinfo(PieceKind captured, Castling last_castling, Square last_enpassant, bool enpassant)
{
    if (last_enpassant != NO_SQUARE)
        return enpassant << 14 | 1 << 13 | last_enpassant << 7 | last_castling << 3 | captured;
    return enpassant << 14 | last_castling << 3 | captured;
}

PieceKind captured_piece(MoveInfo moveinfo)
{
    return PieceKind(moveinfo & 0x7);
}

Castling last_castling(MoveInfo moveinfo)
{
    return Castling((moveinfo >> 3) & 0xF);
}

Square last_enpassant_square(MoveInfo moveinfo)
{
    return Square((moveinfo >> 7) & 0x3F);
}

bool last_enpassant(MoveInfo moveinfo)
{
    return (moveinfo >> 13) & 0x1;
}

bool enpassant(MoveInfo moveinfo)
{
    return (moveinfo >> 14) & 0x1;
}

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

    position.zobrist_hash = hash_position(position);

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
            break;
        }
    }
    position.piece_count[piece] -= 1;
}

void move_piece(Position& position, Square from, Square to)
{
    assert(position.board[from] != NO_PIECE);
    assert(position.board[to] == NO_PIECE);

    Piece piece = position.board[from];
    position.board[from] = NO_PIECE;
    position.board[to] = piece;

    Bitboard change = square_bb(from) | square_bb(to);
    position.by_color_bb[get_color(piece)] ^= change;
    position.by_piece_kind_bb[get_piece_kind(piece)] ^= change;

    for (int i = 0; i < position.piece_count[piece]; ++i)
    {
        if (position.piece_position[piece][i] == from)
        {
            position.piece_position[piece][i] = to;
            break;
        }
    }
}


MoveInfo do_move(Position& pos, Move move)
{
    update_hash(pos, move);

    PieceKind captured = NO_PIECE_KIND;
    Castling prev_castling = pos.castling_rights;
    Square prev_enpassant_sq = pos.enpassant;
    bool enpassant = false;

    Color side = pos.current_side;
    pos.current_side = !pos.current_side;

    if (castling(move) != NO_CASTLING)
    {
        Rank rank = side == WHITE ? RANK_1 : RANK_8;
        if (castling(move) == KING_CASTLING)
        {
            move_piece(pos, make_square(rank, FILE_E), make_square(rank, FILE_G));
            move_piece(pos, make_square(rank, FILE_H), make_square(rank, FILE_F));
        }
        else  // castling(move) == QUEEN_CASTLING
        {
            move_piece(pos, make_square(rank, FILE_E), make_square(rank, FILE_C));
            move_piece(pos, make_square(rank, FILE_A), make_square(rank, FILE_D));
        }

        pos.castling_rights = Castling(pos.castling_rights & ~CASTLING_RIGHTS[side]);
        pos.enpassant = NO_SQUARE;

        return create_moveinfo(captured, prev_castling, prev_enpassant_sq, enpassant);
    }

    Piece moved_piece = pos.board[from(move)];
    Piece captured_piece = pos.board[to(move)];
    captured = make_piece_kind(captured_piece);

    assert(moved_piece != NO_PIECE);

    // enpassant
    if (get_piece_kind(moved_piece) == PAWN && to(move) == pos.enpassant)
    {
        move_piece(pos, from(move), to(move));
        Square captured_square = Square(to(move) + (side == WHITE ? -8 : 8));
        remove_piece(pos, captured_square);
        enpassant = true;
    }
    else
    {
        if (captured_piece != NO_PIECE)
            remove_piece(pos, to(move));

        if (promotion(move) != NO_PIECE_KIND)
        {
            remove_piece(pos, from(move));
            add_piece(pos, side, promotion(move), to(move));
        }
        else
            move_piece(pos, from(move), to(move));

        if (get_piece_kind(moved_piece) == KING)
            pos.castling_rights = Castling(pos.castling_rights & ~CASTLING_RIGHTS[side]);
        if (get_piece_kind(moved_piece) == ROOK && from(move) == KING_SIDE_ROOK_SQUARE[side])
            pos.castling_rights = Castling(pos.castling_rights & ~(CASTLING_RIGHTS[side] & KING_CASTLING));
        if (get_piece_kind(moved_piece) == ROOK && from(move) == QUEEN_SIDE_ROOK_SQUARE[side])
            pos.castling_rights = Castling(pos.castling_rights & ~(CASTLING_RIGHTS[side] & QUEEN_CASTLING));
        if (make_piece_kind(captured_piece) == ROOK && to(move) == KING_SIDE_ROOK_SQUARE[!side])
            pos.castling_rights = Castling(pos.castling_rights & ~(CASTLING_RIGHTS[!side] & KING_CASTLING));
        if (make_piece_kind(captured_piece) == ROOK && to(move) == QUEEN_SIDE_ROOK_SQUARE[!side])
            pos.castling_rights = Castling(pos.castling_rights & ~(CASTLING_RIGHTS[!side] & QUEEN_CASTLING));
    }


    Rank enpassant_rank = (side == WHITE) ? RANK_4 : RANK_5;
    Rank rank2 = (side == WHITE) ? RANK_2 : RANK_7;
    if (get_piece_kind(moved_piece) == PAWN && rank(from(move)) == rank2 && rank(to(move)) == enpassant_rank)
        pos.enpassant = Square(to(move) + (side == WHITE ? -8 : 8));
    else
        pos.enpassant = NO_SQUARE;

    return create_moveinfo(captured, prev_castling, prev_enpassant_sq, enpassant);
}

void undo_move(Position& pos, Move move, MoveInfo moveinfo)
{
    pos.current_side = !pos.current_side;
    Color side = pos.current_side;

    pos.castling_rights = last_castling(moveinfo);
    if (last_enpassant(moveinfo))
        pos.enpassant = last_enpassant_square(moveinfo);
    else
        pos.enpassant = NO_SQUARE;

    if (castling(move) != NO_CASTLING)
    {
        Rank rank = side == WHITE ? RANK_1 : RANK_8;
        if (castling(move) == KING_CASTLING)
        {
            move_piece(pos, make_square(rank, FILE_G), make_square(rank, FILE_E));
            move_piece(pos, make_square(rank, FILE_F), make_square(rank, FILE_H));
        }
        else  // castling(move) == QUEEN_CASTLING
        {
            move_piece(pos, make_square(rank, FILE_C), make_square(rank, FILE_E));
            move_piece(pos, make_square(rank, FILE_D), make_square(rank, FILE_A));
        }

        return;
    }

    Piece captured = make_piece(!side, captured_piece(moveinfo));

    if (enpassant(moveinfo))
    {
        Square captured_square = Square(to(move) + (side == WHITE ? -8 : 8));
        add_piece(pos, !side, PAWN, captured_square);
    }

    if (promotion(move) != NO_PIECE_KIND)
    {
        add_piece(pos, side, PAWN, from(move));
        remove_piece(pos, to(move));
    }
    else
        move_piece(pos, to(move), from(move));

    if (captured != NO_PIECE)
        add_piece(pos, captured, to(move));

    update_hash(pos, move);
}

bool is_in_check(const Position& position)
{
    Color side = position.current_side;
    Bitboard attacked = attacked_squares(position);
    return attacked & pieces_bb(position, side, KING);
}


}
