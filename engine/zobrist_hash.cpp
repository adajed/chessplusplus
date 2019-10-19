#include "types.h"
#include "zobrist_hash.h"

#include <random>

namespace engine
{
namespace zobrist
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

void init()
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

HashKey hash(const Position& position)
{
    HashKey hash = 0ULL;

    if (position.side_to_move() == BLACK)
        hash ^= SIDE_HASH;

    for (Piece piece = W_PAWN; piece <= B_KING; ++piece)
    {
        for (int i = 0; i < position.number_of_pieces(piece); ++i)
        {
            Square square = position.piece_position(piece, i);
            hash ^= PIECE_HASH[piece][square];
        }
    }

    hash ^= CASTLING_HASH[position.castling_rights()];

    if (position.enpassant_square() != NO_SQUARE)
        hash ^= ENPASSANT_HASH[file(position.enpassant_square())];

    return hash;
}

}  // namespace zobrist
}  // namespace engine
