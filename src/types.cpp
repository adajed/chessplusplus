#include "types.h"
#include "bitboard.h"

namespace engine
{

Move create_move(Square from, Square to)
{
    assert(from != NO_SQUARE);
    assert(to != NO_SQUARE);
    return to << 6 | from;
}

Move create_promotion(Square from, Square to, PieceKind promotion)
{
    assert(from != NO_SQUARE);
    assert(to != NO_SQUARE);
    assert(promotion != PAWN);
    assert(promotion != KING);
    return promotion << 12 | to << 6 | from;
}

Move create_castling(Castling castling)
{
    assert(castling == KING_CASTLING || castling == QUEEN_CASTLING);
    return (castling == KING_CASTLING ? 1 : 2) << 15;
}

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

MoveInfo create_moveinfo(PieceKind captured, Castling last_castling, Square last_enpassant,
                         bool enpassant, uint8_t half_move_counter)
{
    if (last_enpassant != NO_SQUARE)
        return half_move_counter << 15 | (!!enpassant) << 14 | 1 << 13 | last_enpassant << 7 | last_castling << 3 | captured;
    return half_move_counter << 15 | enpassant << 14 | last_castling << 3 | captured;
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

uint8_t half_move_counter(MoveInfo moveinfo)
{
    return (moveinfo >> 15) & 0xFF;
}

std::ostream& print_bitboard(std::ostream& stream, Bitboard bb)
{
    stream << "##########" << std::endl;
    for (int rank = 7; rank >= 0; rank--)
    {
        stream << "#";
        for (int file = 0; file < 8; ++file)
        {
            if (bb & square_bb(make_square(Rank(rank), File(file))))
                stream << ".";
            else
                stream << " ";
        }
        stream << "#" << std::endl;
    }
    stream << "##########" << std::endl;
    return stream;
}

}