#include "position.h"
#include "movegen.h"
#include "zobrist_hash.h"

#include <sstream>

namespace engine
{

Square notationToSquare(std::string notation)
{
    File file = File(notation[0] - 'a');
    Rank rank = Rank(notation[1] - '1');
    return make_square(rank, file);
}

Position::Position() : Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
{
}

Position::Position(std::string fen)
{
    _current_side = WHITE;
    std::fill_n(_board, SQUARE_NUM, NO_PIECE);
    std::fill_n(_piece_count, PIECE_NUM, 0);
    std::fill_n(_by_piece_kind_bb, PIECE_KIND_NUM, 0ULL);
    std::fill_n(_by_color_bb, COLOR_NUM, 0ULL);
    _castling_rights = NO_CASTLING;
    _enpassant_square = NO_SQUARE;

    std::istringstream stream(fen);
    std::string token;

    std::map<char, Piece> char_to_piece = {
        {'P', W_PAWN}, {'N', W_KNIGHT}, {'B', W_BISHOP}, {'R', W_ROOK}, {'Q', W_QUEEN}, {'K', W_KING},
        {'p', B_PAWN}, {'n', B_KNIGHT}, {'b', B_BISHOP}, {'r', B_ROOK}, {'q', B_QUEEN}, {'k', B_KING},
    };

    stream >> token;
    Square square = SQ_A8;
    for (char c : token)
    {
        if (c == '/')
            square -= 16;
        else if ('0' <= c && c <= '9')
            square += (c - '0');
        else
        {
            Piece piece = char_to_piece[c];
            _board[square] = piece;
            _by_color_bb[get_color(piece)] |= square_bb(square);
            _by_piece_kind_bb[get_piece_kind(piece)] |= square_bb(square);
            _piece_position[piece][_piece_count[piece]++] = square;

            ++square;
        }
    }

    stream >> token;
    _current_side = token == "w" ? WHITE : BLACK;

    stream >> token;
    for (char c : token)
    {
        switch (c)
        {
            case 'K': _castling_rights |= W_OO;  break;
            case 'Q': _castling_rights |= W_OOO; break;
            case 'k': _castling_rights |= B_OO;  break;
            case 'q': _castling_rights |= B_OOO; break;
        }
    }

    stream >> token;
    _enpassant_square = token == "-" ? NO_SQUARE : notationToSquare(token);

    stream >> _half_move_counter;
    stream >> _move_counter;

    _zobrist_hash = zobrist::hash(*this);
}

bool Position::operator== (const Position& other) const
{
    if (_current_side != other._current_side)
        return false;
    if (_castling_rights != other._castling_rights)
        return false;
    if (_enpassant_square != other._enpassant_square)
        return false;
    for (Square square = SQ_A1; square <= SQ_H8; ++square)
        if (_board[square] != other._board[square])
            return false;
    return true;
}

std::string Position::fen() const
{
    const std::string piece_to_char = " PNBRQKpnbrqk";

    std::ostringstream stream;

    int counter = 0;
    for (Rank rank = RANK_8; rank >= RANK_1; --rank)
    {
        for (File file = FILE_A; file <= FILE_H; ++file)
        {
            Piece piece = piece_at(make_square(rank, file));
            if (piece == NO_PIECE)
                counter++;
            else
            {
                if (counter > 0)
                {
                    stream << static_cast<char>('0' + counter);
                    counter = 0;
                }
                stream << piece_to_char[piece];
            }
        }
        if (counter > 0)
        {
            stream << static_cast<char>('0' + counter);
            counter = 0;
        }
        if (rank > 0)
            stream << "/";
    }
    stream << " " << (_current_side == WHITE ? "w" : "b") << " ";
    if (_castling_rights != NO_CASTLING)
    {
        if (_castling_rights & W_OO)  stream << "K";
        if (_castling_rights & W_OOO) stream << "Q";
        if (_castling_rights & B_OO)  stream << "k";
        if (_castling_rights & B_OOO) stream << "q";
    }
    else
        stream << "-";
    stream << " ";
    if (_enpassant_square != NO_SQUARE)
    {
        stream << char('a' + file(_enpassant_square));
        stream << char('1' + rank(_enpassant_square));
    }
    else
        stream << "-";

    stream << " " << _half_move_counter << " " << _move_counter;

    return stream.str();
}

Bitboard Position::pieces() const
{
    return _by_color_bb[WHITE] | _by_color_bb[BLACK];
}

Bitboard Position::pieces(Color c) const
{
    return _by_color_bb[c];
}

Bitboard Position::pieces(PieceKind p) const
{
    return _by_piece_kind_bb[p];
}

Bitboard Position::pieces(Color c, PieceKind p) const
{
    return _by_color_bb[c] & _by_piece_kind_bb[p];
}

void Position::add_piece(Piece piece, Square square)
{
    assert(_board[square] == NO_PIECE);

    _board[square] = piece;
    _by_color_bb[get_color(piece)] |= square_bb(square);
    _by_piece_kind_bb[get_piece_kind(piece)] |= square_bb(square);
    _piece_position[piece][_piece_count[piece]] = square;
    _piece_count[piece] += 1;

    _zobrist_hash ^= zobrist::PIECE_HASH[piece][square];
}

void Position::remove_piece(Square square)
{
    assert(_board[square] != NO_PIECE);

    Piece piece = _board[square];
    _board[square] = NO_PIECE;
    _by_color_bb[get_color(piece)] ^= square_bb(square);
    _by_piece_kind_bb[get_piece_kind(piece)] ^= square_bb(square);

    for (int i = 0; i < _piece_count[piece] - 1; ++i)
    {
        if (_piece_position[piece][i] == square)
        {
            int pos = _piece_count[piece] - 1;
            _piece_position[piece][i] = _piece_position[piece][pos];
            break;
        }
    }
    _piece_count[piece] -= 1;

    _zobrist_hash ^= zobrist::PIECE_HASH[piece][square];
}

void Position::move_piece(Square from, Square to)
{
    assert(_board[from] != NO_PIECE);
    assert(_board[to] == NO_PIECE);

    Piece piece = _board[from];
    _board[from] = NO_PIECE;
    _board[to] = piece;

    Bitboard change = square_bb(from) | square_bb(to);
    _by_color_bb[get_color(piece)] ^= change;
    _by_piece_kind_bb[get_piece_kind(piece)] ^= change;

    for (int i = 0; i < _piece_count[piece]; ++i)
    {
        if (_piece_position[piece][i] == from)
        {
            _piece_position[piece][i] = to;
            break;
        }
    }

    _zobrist_hash ^= zobrist::PIECE_HASH[piece][from];
    _zobrist_hash ^= zobrist::PIECE_HASH[piece][from];
}

void Position::change_current_side()
{
    _zobrist_hash ^= zobrist::SIDE_HASH;
    _current_side = !_current_side;
}

MoveInfo Position::do_move(Move move)
{
    PieceKind captured = NO_PIECE_KIND;
    Castling prev_castling = _castling_rights;
    Square prev_enpassant_sq = _enpassant_square;
    bool enpassant = false;

    Color side = _current_side;
    change_current_side();

    if (_enpassant_square != NO_SQUARE)
        _zobrist_hash ^= zobrist::ENPASSANT_HASH[file(_enpassant_square)];

    if (castling(move) != NO_CASTLING)
    {
        Rank rank = side == WHITE ? RANK_1 : RANK_8;
        if (castling(move) == KING_CASTLING)
        {
            move_piece(make_square(rank, FILE_E), make_square(rank, FILE_G));
            move_piece(make_square(rank, FILE_H), make_square(rank, FILE_F));
        }
        else  // castling(move) == QUEEN_CASTLING
        {
            move_piece(make_square(rank, FILE_E), make_square(rank, FILE_C));
            move_piece(make_square(rank, FILE_A), make_square(rank, FILE_D));
        }

        _zobrist_hash ^= zobrist::CASTLING_HASH[_castling_rights];
        _castling_rights &= !CASTLING_RIGHTS[side];
        _zobrist_hash ^= zobrist::CASTLING_HASH[_castling_rights];
        _enpassant_square = NO_SQUARE;

        return create_moveinfo(captured, prev_castling, prev_enpassant_sq, enpassant);
    }

    Piece moved_piece = _board[from(move)];
    Piece captured_piece = _board[to(move)];
    captured = make_piece_kind(captured_piece);

    assert(moved_piece != NO_PIECE);

    // enpassant
    if (get_piece_kind(moved_piece) == PAWN && to(move) == _enpassant_square)
    {
        move_piece(from(move), to(move));
        Square captured_square = Square(to(move) + (side == WHITE ? -8 : 8));
        remove_piece(captured_square);
        enpassant = true;
    }
    else
    {
        if (captured_piece != NO_PIECE)
            remove_piece(to(move));

        if (promotion(move) != NO_PIECE_KIND)
        {
            remove_piece(from(move));
            add_piece(make_piece(side, promotion(move)), to(move));
        }
        else
            move_piece(from(move), to(move));

        _zobrist_hash ^= zobrist::CASTLING_HASH[_castling_rights];
        if (get_piece_kind(moved_piece) == KING)
            _castling_rights &= !CASTLING_RIGHTS[side];
        if (get_piece_kind(moved_piece) == ROOK && from(move) == KING_SIDE_ROOK_SQUARE[side])
            _castling_rights &= !(CASTLING_RIGHTS[side] & KING_CASTLING);
        if (get_piece_kind(moved_piece) == ROOK && from(move) == QUEEN_SIDE_ROOK_SQUARE[side])
            _castling_rights &= !(CASTLING_RIGHTS[side] & QUEEN_CASTLING);
        if (make_piece_kind(captured_piece) == ROOK && to(move) == KING_SIDE_ROOK_SQUARE[!side])
            _castling_rights &= !(CASTLING_RIGHTS[!side] & KING_CASTLING);
        if (make_piece_kind(captured_piece) == ROOK && to(move) == QUEEN_SIDE_ROOK_SQUARE[!side])
            _castling_rights &= !(CASTLING_RIGHTS[!side] & QUEEN_CASTLING);
        _zobrist_hash ^= zobrist::CASTLING_HASH[_castling_rights];
    }


    Rank enpassant_rank = (side == WHITE) ? RANK_4 : RANK_5;
    Rank rank2 = (side == WHITE) ? RANK_2 : RANK_7;
    if (get_piece_kind(moved_piece) == PAWN && rank(from(move)) == rank2 && rank(to(move)) == enpassant_rank)
    {
        _enpassant_square = Square(to(move) + (side == WHITE ? -8 : 8));
        _zobrist_hash ^= zobrist::ENPASSANT_HASH[file(_enpassant_square)];
    }
    else
        _enpassant_square = NO_SQUARE;

    return create_moveinfo(captured, prev_castling, prev_enpassant_sq, enpassant);
}

void Position::undo_move(Move move, MoveInfo moveinfo)
{
    change_current_side();
    Color side = _current_side;

    if (_enpassant_square != NO_SQUARE)
        _zobrist_hash ^= zobrist::ENPASSANT_HASH[file(_enpassant_square)];

    _zobrist_hash ^= zobrist::CASTLING_HASH[_castling_rights];
    _castling_rights = last_castling(moveinfo);
    _zobrist_hash ^= zobrist::CASTLING_HASH[_castling_rights];

    if (last_enpassant(moveinfo))
    {
        _enpassant_square = last_enpassant_square(moveinfo);
        _zobrist_hash ^= zobrist::ENPASSANT_HASH[file(_enpassant_square)];
    }
    else
        _enpassant_square = NO_SQUARE;

    if (castling(move) != NO_CASTLING)
    {
        Rank rank = side == WHITE ? RANK_1 : RANK_8;
        if (castling(move) == KING_CASTLING)
        {
            move_piece(make_square(rank, FILE_G), make_square(rank, FILE_E));
            move_piece(make_square(rank, FILE_F), make_square(rank, FILE_H));
        }
        else  // castling(move) == QUEEN_CASTLING
        {
            move_piece(make_square(rank, FILE_C), make_square(rank, FILE_E));
            move_piece(make_square(rank, FILE_D), make_square(rank, FILE_A));
        }

        return;
    }

    Piece captured = make_piece(!side, captured_piece(moveinfo));

    if (enpassant(moveinfo))
    {
        Square captured_square = Square(to(move) + (side == WHITE ? -8 : 8));
        add_piece(make_piece(!side, PAWN), captured_square);
    }

    if (promotion(move) != NO_PIECE_KIND)
    {
        add_piece(make_piece(side, PAWN), from(move));
        remove_piece(to(move));
    }
    else
        move_piece(to(move), from(move));

    if (captured != NO_PIECE)
        add_piece(captured, to(move));
}

bool Position::is_in_check(Color side) const
{
    Bitboard attacked = attacked_squares(*this, side);
    return attacked & pieces(side, KING);
}

bool Position::is_checkmate() const
{
    Move* begin = MOVE_LIST[0];
    Move* end = generate_moves(*this, _current_side, begin);

    return (begin == end) && is_in_check(_current_side);
}

GamePhase Position::game_phase() const
{
    uint32_t value[] = {0, 0};

    uint32_t piece_values[] = {0, 0, 3, 3, 5, 9, 0};

    for (Color side : {WHITE, BLACK})
        for (PieceKind p = KNIGHT; p < KING; ++p)
            value[side] += piece_values[p] * _piece_count[p];

    return (value[WHITE] < 18 && value[BLACK] < 18) ? END_GAME : MIDDLE_GAME;
}

std::ostream& operator<< (std::ostream& stream, const Position& position)
{
    const std::string piece_to_char = ".PNBRQKpnbrqk";

    for (Rank rank = RANK_8; rank >= RANK_1; --rank)
    {
        stream << (rank + 1) << "  ";
        for (File file = FILE_A; file <= FILE_H; ++file)
        {
            Piece piece = position.piece_at(make_square(rank, file));
            stream << piece_to_char[piece] << " ";
        }
        stream << std::endl;
    }
    stream << std::endl;
    stream << "   A B C D E F G H" << std::endl;
    stream << std::endl;
    stream << "Fen: \"" << position.fen() << "\"" << std::endl;
    if (position.side_to_move() == WHITE)
        stream << "White to move" << std::endl;
    else
        stream << "Black to move" << std::endl;

    return stream;
}


}
