#include "position.h"
#include "types.h"
#include "zobrist_hash.h"

#include <random>

namespace engine
{

uint64_t PIECE_HASH[PIECE_NUM][SQUARE_NUM];
uint64_t CASTLING_HASH[1 << 4];
uint64_t SIDE_HASH;
uint64_t ENPASSANT_HASH[FILE_NUM];

namespace
{

uint64_t random_uint64()
{
    static std::random_device rd;
    static std::mt19937_64 eng(rd());
    static std::uniform_int_distribution<uint64_t> dist;

    return dist(eng);
}

}  // namespace

void init_zobrist()
{
    for (Piece piece = Piece(0); piece < PIECE_NUM; ++piece)
        for (Square square = SQ_A1; square <= SQ_H8; ++square)
            PIECE_HASH[piece][square] = random_uint64();

    for (int i = 0; i < 1 << 4; ++i)
        CASTLING_HASH[i] = random_uint64();

    SIDE_HASH = random_uint64();

    for (File file = FILE_A; file <= FILE_H; ++file)
        ENPASSANT_HASH[file] = random_uint64();
}

HashKey::HashKey()
    : _piece_key(0ULL), _pawn_key(0ULL), _enpassant_key(0ULL), _castling_key(0ULL), _color_key(0ULL)
{
}

void HashKey::init(const Position& position)
{
    if (position.side_to_move() == BLACK)
        _color_key ^= SIDE_HASH;

    for (Color color : {WHITE, BLACK})
    {
        Piece piece = make_piece(color, PAWN);
        for (int i = 0; i < position.number_of_pieces(piece); ++i)
        {
            Square square = position.piece_position(piece, i);
            _pawn_key ^= PIECE_HASH[piece][square];
        }
    }

    for (Color color : {WHITE, BLACK})
    {
        for (PieceKind piece_kind : {KNIGHT, BISHOP, ROOK, QUEEN, KING})
        {
            Piece piece = make_piece(color, piece_kind);
            for (int i = 0; i < position.number_of_pieces(piece); ++i)
            {
                Square square = position.piece_position(piece, i);
                _piece_key ^= PIECE_HASH[piece][square];
            }
        }
    }

    _castling_key ^= CASTLING_HASH[position.castling_rights()];

    if (position.enpassant_square() != NO_SQUARE)
        _enpassant_key ^= ENPASSANT_HASH[file(position.enpassant_square())];
}

uint64_t HashKey::get_key() const
{
    return _piece_key ^ _pawn_key ^ _enpassant_key ^ _castling_key ^ _color_key;
}

uint64_t HashKey::get_pawnkey() const
{
    return _pawn_key;
}

void HashKey::move_piece(Piece piece, Square from, Square to)
{
    toggle_piece(piece, from);
    toggle_piece(piece, to);
}

void HashKey::toggle_piece(Piece piece, Square sq)
{
    if (get_piece_kind(piece) == PAWN)
        _pawn_key ^= PIECE_HASH[piece][sq];
    else
        _piece_key ^= PIECE_HASH[piece][sq];
}

void HashKey::flip_side()
{
    _color_key ^= SIDE_HASH;
}

void HashKey::clear_enpassant()
{
    _enpassant_key = 0ULL;
}

void HashKey::set_enpassant(File file)
{
    _enpassant_key = ENPASSANT_HASH[file];
}

void HashKey::clear_castling()
{
    _castling_key = 0ULL;
}

void HashKey::set_castling(Castling castling)
{
    _castling_key = CASTLING_HASH[castling];
}


}  // namespace engine
