#include "bitboard.h"
#include "bithacks.h"
#include "endgame.h"
#include "position.h"
#include "position_bitboards.h"
#include "score.h"
#include "types.h"

namespace engine
{

#define S(mg, eg) (Score((mg), (eg)))

const Value PIECE_WEIGHTS[PIECE_KIND_NUM] = {0, 0, 1, 1, 2, 4, 0};

const Value MAX_PIECE_WEIGTHS = 2 * (  2 * PIECE_WEIGHTS[KNIGHT]
                                       + 2 * PIECE_WEIGHTS[BISHOP]
                                       + 2 * PIECE_WEIGHTS[ROOK]
                                       + 1 * PIECE_WEIGHTS[QUEEN]);

const Score PIECE_VALUE[PIECE_KIND_NUM] = {
    S(0, 0), S(300, 370), S(890, 890), S(950, 900), S(1500, 1500), S(2700, 2700), S(0, 0)
};

const Score MOBILITY_BONUS[PIECE_KIND_NUM] =
{
    S(0, 0), S(5, 10), S(6, 12), S(9, 4), S(2, 12), S(2, 6), S(0, 2)
};

// bonus for rook on semiopen file
const Score ROOK_SEMIOPEN_FILE_BONUS = S(10, 11);

// bonus for rook on open file
const Score ROOK_OPEN_FILE_BONUS = S(20, 40);

// bonus for bishop pair
const Score BISHOP_PAIR_BONUS = S(50, 60);

// bonus for passed pawn
const Score PASSED_PAWN_BONUS = S(15, 150);

// bonus for connecting rooks
const Score CONNECTED_ROOKS_BONUS = S(40, 20);

const Score OUTPOST_BONUS = S(20, 10);

const Score OUTPOST_KNIGHT_BONUS = S(100, 50);

const Score OUTPOST_BISHOP_BONUS = S(100, 50);

// penalty for double pawns
const Score DOUBLE_PAWN_PENALTY = S(-15, -45);

// penalty for tripled pawns
const Score TRIPLE_PAWN_PENALTY = S(-35, -100);

const Score PAWN_CHAIN_BONUS[FILE_NUM] = {
    S(0, 0), S(0, 0), S(2, 4), S(4, 10), S(5, 15), S(5, 20), S(6, 20), S(7, 20)
};

const Score CONNECTED_PAWNS_BONUS[FILE_NUM + 1] = {
    S(0, 0), S(-40, -80), S(5, 10), S(8, 18), S(10, 25), S(12, 30), S(12, 30), S(12, 30), S(12, 30)
};

const Score BACKWARD_PAWN_PENALTY = S(-30, -100);

const Score KING_SAFETY_BONUS = S(20, 0);

const Score SAFE_KNIGHT = S(10, 2);
const Score CONTROL_CENTER_KNIGHT = S(10, 10);


Square flip(Square square)
{
    return make_square(RANK_8 - rank(square), file(square));
}

PositionScorer::PositionScorer()
    : _pawn_hash_table(), _weight(-1)
{
}

void PositionScorer::clear()
{
    _pawn_hash_table.clear();
}

Value PositionScorer::combine(const Score& score)
{
    assert(0 <= _weight && _weight <= MAX_PIECE_WEIGTHS);
    return (score.mg * _weight + score.eg * (MAX_PIECE_WEIGTHS - _weight)) / MAX_PIECE_WEIGTHS;
}

Value PositionScorer::score(const Position& position)
{
    if (!popcount_more_than_one(position.pieces(WHITE)))
        return endgame::score_endgame<BLACK>(position);
    if (!popcount_more_than_one(position.pieces(BLACK)))
        return endgame::score_endgame<WHITE>(position);

    setup<WHITE>(position);
    setup<BLACK>(position);

    for (Color side : {WHITE, BLACK})
    {
        for (PieceKind piecekind : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING})
        {
            move_count[side][piecekind] = 0;
        }
        Move* begin = move_list;
        Move* end = generate_moves(position, side, begin);
        for (Move* it = begin; it != end; ++it)
        {
            PieceKind piecekind = make_piece_kind(position.piece_at(from(*it)));
            move_count[side][piecekind]++;
        }
    }

    _weight = game_phase_weight(position);

    Score pieces = score_pieces(position);
    Score pawns = score_pawns(position);
    Value value = combine(pawns + pieces);

    return position.side_to_move() == WHITE ? value : -value;
}

template <Color side>
void PositionScorer::setup(const Position& position)
{
    _attacked_by_bb[side][PAWN] = pawn_attacks<side>(position.pieces(make_piece(side, PAWN)));

    _attacked_by_bb[side][KNIGHT] = 0ULL;
    for (int i = 0; i < position.number_of_pieces(make_piece(side, KNIGHT)); ++i)
    {
        _attacked_by_bb[side][KNIGHT] |= KNIGHT_MASK[position.piece_position(make_piece(side, KNIGHT), i)];
    }

    _attacked_by_bb[side][BISHOP] = 0ULL;
    for (int i = 0; i < position.number_of_pieces(make_piece(side, BISHOP)); ++i)
    {
        Square sq = position.piece_position(make_piece(side, BISHOP), i);
        // remove own bishops and queens from blockers (x-ray attack)
        Bitboard blockers = position.pieces() & (~position.pieces(side, BISHOP, QUEEN));
        _attacked_by_bb[side][BISHOP] |= slider_attack<BISHOP>(sq, blockers);
    }

    _attacked_by_bb[side][ROOK] = 0ULL;
    for (int i = 0; i < position.number_of_pieces(make_piece(side, ROOK)); ++i)
    {
        Square sq = position.piece_position(make_piece(side, ROOK), i);
        // remove own rooks and queens from blockers (x-ray attack)
        Bitboard blockers = position.pieces() & (~position.pieces(side, ROOK, QUEEN));
        _attacked_by_bb[side][ROOK] |= slider_attack<ROOK>(sq, blockers);
    }

    _attacked_by_bb[side][QUEEN] = 0ULL;
    for (int i = 0; i < position.number_of_pieces(make_piece(side, QUEEN)); ++i)
    {
        Square sq = position.piece_position(make_piece(side, QUEEN), i);
        // remove own bishops and queens from blockers (x-ray attack)
        Bitboard blockers = position.pieces() & (~position.pieces(side, BISHOP, QUEEN));
        _attacked_by_bb[side][QUEEN] |= slider_attack<BISHOP>(sq, blockers);
        // remove own bishops and rooks from blockers (x-ray attack)
        blockers = position.pieces() & (~position.pieces(side, ROOK, QUEEN));
        _attacked_by_bb[side][QUEEN] |= slider_attack<ROOK>(sq, blockers);
    }

    _attacked_by_piece[side] = _attacked_by_bb[side][KNIGHT]
                             | _attacked_by_bb[side][BISHOP]
                             | _attacked_by_bb[side][ROOK]
                             | _attacked_by_bb[side][QUEEN];

    _outposts_bb[side] = get_outposts<side>(position);
}

Score PositionScorer::score_pieces(const Position& position)
{
    Score w = score_pieces_for_side<WHITE>(position);
    Score b = score_pieces_for_side<BLACK>(position);

    return w - b;
}

template <Color side>
Score PositionScorer::score_pieces_for_side(const Position& position)
{
    Score value;

#define FOR_EACH_PIECE(piece, code)                             \
    {                                                           \
    constexpr Piece pieceT = make_piece(side, piece);           \
    int no_pieces = position.number_of_pieces(pieceT);          \
    value += PIECE_VALUE[piece] * Value(no_pieces);             \
    for (int i = 0; i < no_pieces; ++i)                         \
    {                                                           \
        Square sq = position.piece_position(pieceT, i);         \
        sq = side == WHITE ? sq : flip(sq);                     \
        code;                                                   \
    }                                                           \
    }

    /* FOR_EACH_PIECE(KNIGHT, ;); */
    FOR_EACH_PIECE(BISHOP, ;);
    FOR_EACH_PIECE(ROOK, ;);
    FOR_EACH_PIECE(QUEEN, ;);


    // knights
    {
        constexpr Piece piece = make_piece(side, KNIGHT);
        Bitboard opponent_pieces = position.pieces(!side) & ~position.pieces(!side, PAWN);
        int no_pieces = position.number_of_pieces(piece);

        value += PIECE_VALUE[KNIGHT] * Value(no_pieces);
        for (int i = 0; i < no_pieces; ++i)
        {
            Square sq = position.piece_position(piece, i);
            Bitboard attacking = KNIGHT_MASK[sq];

            if (square_bb(sq) & _attacked_by_bb[side][PAWN])
            {
                value += SAFE_KNIGHT;
            }
            value += CONTROL_CENTER_KNIGHT * Value(popcount(attacking & center_bb));


            Bitboard moves = attacking;
            // cannot move into square with our piece
            moves &= ~position.pieces(side);
            // attacked by opponents pawns
            moves &= ~(_attacked_by_bb[!side][PAWN] & ~opponent_pieces);
            // attacked by opponents piece and not defended by our pawns
            moves &= ~(_attacked_by_piece[!side] & ~_attacked_by_bb[side][PAWN]);
            value += MOBILITY_BONUS[KNIGHT] * Value(popcount(moves));

            if (_outposts_bb[side] & square_bb(sq))
            {
                value += OUTPOST_KNIGHT_BONUS;
            }
        }
    }

    constexpr Piece ROOK_PIECE = make_piece(side, ROOK);
    for (int i = 0; i < position.number_of_pieces(ROOK_PIECE); ++i)
    {
        Square square = position.piece_position(ROOK_PIECE, i);
        Bitboard file_bb = FILES_BB[file(square)];

        if (!(file_bb & position.pieces(PAWN)))
            value += ROOK_OPEN_FILE_BONUS;
        if (!(file_bb & position.pieces(side, PAWN)) &&
                file_bb & position.pieces(!side, PAWN))
            value += ROOK_SEMIOPEN_FILE_BONUS;
    }

    if (position.number_of_pieces(ROOK_PIECE) == 2)
    {
        Square rook1 = position.piece_position(ROOK_PIECE, 0);
        Square rook2 = position.piece_position(ROOK_PIECE, 1);

        if (file(rook1) == file(rook2) || rank(rook1) == rank(rook2))
        {
            Bitboard path = LINES[rook1][rook2] & ~square_bb(rook1) & ~square_bb(rook2);
            if (!(position.pieces() & path))
                value += CONNECTED_ROOKS_BONUS;
        }
    }

    if ((position.pieces(side, BISHOP) & white_squares_bb) &&
            (position.pieces(side, BISHOP) & black_squares_bb))
        value += BISHOP_PAIR_BONUS;

    Bitboard bishop_outpost_bb = _outposts_bb[side] & position.pieces(side, BISHOP);
    value += Value(popcount(bishop_outpost_bb)) * OUTPOST_BISHOP_BONUS;

    constexpr Rank first_rank  = side == WHITE ? RANK_1 : RANK_8;
    constexpr Rank second_rank = side == WHITE ? RANK_2 : RANK_7;
    Square kingSq = position.piece_position(make_piece(side, KING), 0);
    if (rank(kingSq) == first_rank)
    {
        Bitboard safetyPawns = (NEIGHBOUR_FILES_BB[file(kingSq)] | FILES_BB[file(kingSq)]) & RANKS_BB[second_rank] & position.pieces(side, PAWN);
        value += popcount(safetyPawns) * KING_SAFETY_BONUS;
    }

    return value;
}

Score PositionScorer::score_pawns(const Position& position)
{
    std::pair<Value, Value> p;
    Score value;

    // check in hash table
    if (!_pawn_hash_table.get(position, p))
    {
        value = score_pawns_for_side<WHITE>(position) - score_pawns_for_side<BLACK>(position);
        _pawn_hash_table.update(position, std::make_pair(value.mg, value.eg));
    }
    else
    {
        value = Score(p.first, p.second);
    }

    return value;
}

template <Color side>
Score PositionScorer::score_pawns_for_side(const Position& position)
{
    constexpr Piece pawn = make_piece(side, PAWN);

    Score value;

    int num_pieces = position.number_of_pieces(pawn);
    value += PIECE_VALUE[PAWN] * Value(num_pieces);
    value += MOBILITY_BONUS[PAWN] * Value(move_count[side][PAWN]);

    std::vector<int> pawn_on_file = {0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < position.number_of_pieces(pawn); ++i)
    {
        Square square = position.piece_position(pawn, i);
        pawn_on_file[file(square)] += 1;

        if (!(passed_pawn_bb<side>(square) & position.pieces(!side, PAWN)))
            value += PASSED_PAWN_BONUS;
    }

    /* int s = 0, e = 0; */
    /* for (; e < 8; ++e) */
    /* { */
    /*     if (pawn_on_file[e] == 0) */
    /*     { */
    /*         value += CONNECTED_PAWNS_BONUS[phase][e - s]; */
    /*         s = e; */
    /*     } */
    /* } */
    /* value += CONNECTED_PAWNS_BONUS[phase][e - s]; */

    for (int i = 0; i < 8; ++i)
    {
        if (pawn_on_file[i] == 2)
            value += DOUBLE_PAWN_PENALTY;
        if (pawn_on_file[i] > 2)
            value += TRIPLE_PAWN_PENALTY;
    }

    Bitboard backpawns = backward_pawns<side>(position.pieces(side, PAWN),
                                              position.pieces(!side, PAWN));
    value += BACKWARD_PAWN_PENALTY * Value(popcount(backpawns));

    return value;
}

Value PositionScorer::game_phase_weight(const Position& position)
{
    Value weight = 0;
    weight += position.number_of_pieces(W_KNIGHT) * PIECE_WEIGHTS[KNIGHT];
    weight += position.number_of_pieces(W_BISHOP) * PIECE_WEIGHTS[BISHOP];
    weight += position.number_of_pieces(W_ROOK) * PIECE_WEIGHTS[ROOK];
    weight += position.number_of_pieces(W_QUEEN) * PIECE_WEIGHTS[QUEEN];
    weight += position.number_of_pieces(B_KNIGHT) * PIECE_WEIGHTS[KNIGHT];
    weight += position.number_of_pieces(B_BISHOP) * PIECE_WEIGHTS[BISHOP];
    weight += position.number_of_pieces(B_ROOK) * PIECE_WEIGHTS[ROOK];
    weight += position.number_of_pieces(B_QUEEN) * PIECE_WEIGHTS[QUEEN];

    return std::min(weight, MAX_PIECE_WEIGTHS);
}

}
