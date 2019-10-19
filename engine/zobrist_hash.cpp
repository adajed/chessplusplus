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

void init_zobrist_hash()
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

HashKey hash_position(const Position& position)
{
    HashKey hash = 0ULL;

    if (position.current_side == BLACK)
        hash ^= SIDE_HASH;

    for (Piece piece = W_PAWN; piece < PIECE_NUM; ++piece)
    {
        for (int i = 0; i < position.piece_count[piece]; ++i)
        {
            Square square = position.piece_position[piece][i];
            hash ^= PIECE_HASH[piece][square];
        }
    }

    hash ^= CASTLING_HASH[position.castling_rights];

    if (position.enpassant != NO_SQUARE)
        hash ^= ENPASSANT_HASH[file(position.enpassant)];

    return hash;
}

void update_hash(Position& position, Move move)
{
    Color side = position.current_side;

    HashKey hash = position.zobrist_hash;
    hash ^= SIDE_HASH;

    if (castling(move) != NO_CASTLING)
    {
        Piece rook = make_piece(side, ROOK);
        Piece king = make_piece(side, KING);
        Rank rank = side == WHITE ? RANK_1 : RANK_8;

        if (castling(move) == KING_CASTLING)
        {
            hash ^= PIECE_HASH[rook][make_square(rank, FILE_H)];
            hash ^= PIECE_HASH[rook][make_square(rank, FILE_F)];
            hash ^= PIECE_HASH[king][make_square(rank, FILE_E)];
            hash ^= PIECE_HASH[king][make_square(rank, FILE_G)];
        }
        else
        {
            hash ^= PIECE_HASH[rook][make_square(rank, FILE_A)];
            hash ^= PIECE_HASH[rook][make_square(rank, FILE_D)];
            hash ^= PIECE_HASH[king][make_square(rank, FILE_E)];
            hash ^= PIECE_HASH[king][make_square(rank, FILE_C)];
        }

        Castling castling = position.castling_rights;
        hash ^= CASTLING_HASH[castling];

        castling = Castling(castling & ~CASTLING_RIGHTS[side]);
        hash ^= CASTLING_HASH[castling];

        position.zobrist_hash = hash;
        return;
    }

    Piece moved_piece = position.board[from(move)];
    Piece captured_piece = position.board[to(move)];

    hash ^= PIECE_HASH[moved_piece][from(move)];
    if (promotion(move) != NO_PIECE_KIND)
        hash ^= PIECE_HASH[make_piece(side, promotion(move))][to(move)];
    else
        hash ^= PIECE_HASH[moved_piece][to(move)];

    if (captured_piece != NO_PIECE)
        hash ^= PIECE_HASH[captured_piece][to(move)];

    Rank enpassant_rank_start = side == WHITE ? RANK_2 : RANK_7;
    Rank enpassant_rank_end   = side == WHITE ? RANK_4 : RANK_5;

    if (position.enpassant != NO_SQUARE)
        hash ^= ENPASSANT_HASH[file(position.enpassant)];
    if (make_piece_kind(moved_piece) == PAWN &&
            rank(from(move)) == enpassant_rank_start && 
            rank(to(move)) == enpassant_rank_end)
        hash ^= ENPASSANT_HASH[file(from(move))];

    Castling castling = position.castling_rights;
    hash ^= CASTLING_HASH[castling];

    if (get_piece_kind(moved_piece) == KING)
        castling = Castling(castling & ~CASTLING_RIGHTS[side]);
    if (get_piece_kind(moved_piece) == ROOK && from(move) == KING_SIDE_ROOK_SQUARE[side])
        castling = Castling(castling & ~(CASTLING_RIGHTS[side] & KING_CASTLING));
    if (get_piece_kind(moved_piece) == ROOK && from(move) == QUEEN_SIDE_ROOK_SQUARE[side])
        castling = Castling(castling & ~(CASTLING_RIGHTS[side] & QUEEN_CASTLING));
    if (make_piece_kind(captured_piece) == ROOK && to(move) == KING_SIDE_ROOK_SQUARE[!side])
        castling = Castling(castling & ~(CASTLING_RIGHTS[!side] & KING_CASTLING));
    if (make_piece_kind(captured_piece) == ROOK && to(move) == QUEEN_SIDE_ROOK_SQUARE[!side])
        castling = Castling(castling & ~(CASTLING_RIGHTS[!side] & QUEEN_CASTLING));

    hash ^= CASTLING_HASH[castling];

    position.zobrist_hash = hash;
}

/*
void update_hash_after_undo_move(Position& position, Move move,
                                 MoveInfo moveinfo)
{
    Color side = position.current_side;

    HashKey hash = position.zobrist_hash;
    hash ^= SIDE_HASH;

    if (castling(move) != NO_CASTLING)
    {
        Piece rook = make_piece(side, ROOK);
        Piece king = make_piece(side, KING);
        Rank rank = side == WHITE ? RANK_1 : RANK_8;

        if (castling(move) == KING_CASTLING)
        {
            hash ^= PIECE_HASH[rook][make_square(rank, FILE_H)];
            hash ^= PIECE_HASH[rook][make_square(rank, FILE_F)];
            hash ^= PIECE_HASH[king][make_square(rank, FILE_E)];
            hash ^= PIECE_HASH[king][make_square(rank, FILE_G)];
        }
        else
        {
            hash ^= PIECE_HASH[rook][make_square(rank, FILE_A)];
            hash ^= PIECE_HASH[rook][make_square(rank, FILE_D)];
            hash ^= PIECE_HASH[king][make_square(rank, FILE_E)];
            hash ^= PIECE_HASH[king][make_square(rank, FILE_C)];
        }

        Castling castling = position.castling_rights;
        hash ^= CASTLING_HASH[castling];

        castling &= ~CASTLING_RIGHTS[side];
        hash ^= CASTLING_HASH[castling];

        position.zobrist_hash = hash;
        return;
    }

    Piece moved_piece = position.board[from(move)];
    Piece captured_piece = position.board[to(move)];

    hash ^= PIECE_HASH[moved_piece][from(move)];
    if (promotion(move) != NO_PIECE_KIND)
        hash ^= PIECE_HASH[make_piece(side, promotion(move))][to(move)];
    else
        hash ^= PIECE_HASH[moved_piece][to(move)];

    if (captured_piece != NO_PIECE)
        hash ^= PIECE_HASH[captured_piece][to(move)];

    Rank enpassant_rank_start = side == WHITE ? RANK_2 : RANK_7;
    Rank enpassant_rank_end   = side == WHITE ? RANK_4 : RANK_5;

    if (position.enpassant != NO_SQUARE)
        hash ^= ENPASSANT_HASH[file(position.enpassant)];
    if (make_piece_kind(moved_piece) == PAWN &&
            rank(from(move)) == enpassant_rank_start && 
            rank(to(move)) == enpassant_rank_end)
        hash ^= ENPASSANT_HASH[file(from(move))];

    Castling castling = position.castling_rights;
    hash ^= CASTLING_HASH[castling];

    if (get_piece_kind(moved_piece) == KING)
        castling = Castling(castling & ~CASTLING_RIGHTS[side]);
    if (get_piece_kind(moved_piece) == ROOK && from(move) == KING_SIDE_ROOK_SQUARE[side])
        castling = Castling(castling & ~(CASTLING_RIGHTS[side] & KING_CASTLING));
    if (get_piece_kind(moved_piece) == ROOK && from(move) == QUEEN_SIDE_ROOK_SQUARE[side])
        castling = Castling(castling & ~(CASTLING_RIGHTS[side] & QUEEN_CASTLING));
    if (make_piece_kind(captured_piece) == ROOK && to(move) == KING_SIDE_ROOK_SQUARE[!side])
        castling = Castling(castling & ~(CASTLING_RIGHTS[!side] & KING_CASTLING));
    if (make_piece_kind(captured_piece) == ROOK && to(move) == QUEEN_SIDE_ROOK_SQUARE[!side])
        castling = Castling(castling & ~(CASTLING_RIGHTS[!side] & QUEEN_CASTLING));

    hash ^= CASTLING_HASH[castling];

    position.zobrist_hash = hash;
}
*/

}  // namespace engine
