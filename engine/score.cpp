#include "score.h"

#include "bitboard.h"
#include "bithacks.h"
#include "endgame.h"
#include "position.h"
#include "position_bitboards.h"
#include "types.h"

namespace engine
{
template <Color side>
Rank relative_rank(Rank r)
{
    return side == WHITE ? r : RANK_8 - r;
}

template <Color side>
Square relative_square(Square sq)
{
    return make_square(relative_rank<side>(rank(sq)), file(sq));
}

#define S(mg, eg) (Score((mg), (eg)))

const Value PIECE_WEIGHTS[PIECE_KIND_NUM] = {0, 0, 1, 1, 2, 4, 0};

const Value MAX_PIECE_WEIGTHS =
    2 * (2 * PIECE_WEIGHTS[KNIGHT] + 2 * PIECE_WEIGHTS[BISHOP] +
         2 * PIECE_WEIGHTS[ROOK] + 1 * PIECE_WEIGHTS[QUEEN]);

const Score PIECE_VALUE[PIECE_KIND_NUM] = {
    S(0, 0),       S(300, 370),   S(890, 890), S(950, 900),
    S(1500, 1500), S(2700, 2700), S(0, 0)};

const Score MOBILITY_BONUS[PIECE_KIND_NUM] = {
    S(0, 0), S(5, 10), S(6, 12), S(9, 4), S(2, 12), S(2, 6), S(0, 5)};

const Score PINNED_PENALTY[PIECE_KIND_NUM] = {
    S(0, 0), S(-20, -20), S(-10, -15), S(-10, -20), S(0, 0), S(0, 0)};

// bonus for rook on semiopen file
const Score ROOK_SEMIOPEN_FILE_BONUS = S(10, 11);

// bonus for rook on open file
const Score ROOK_OPEN_FILE_BONUS = S(20, 40);

// bonus for bishop pair
const Score BISHOP_PAIR_BONUS = S(50, 60);

// bonus for passed pawn
const Score PASSED_PAWN_BONUS = S(20, 40);

// bonus for connecting rooks
const Score CONNECTED_ROOKS_BONUS = S(20, 10);

const Score OUTPOST_BONUS = S(20, 10);

const Score OUTPOST_KNIGHT_BONUS = S(25, 10);

const Score OUTPOST_BISHOP_BONUS = S(20, 10);

// penalty for double pawns
const Score DOUBLE_PAWN_PENALTY = S(-15, -45);

// penalty for tripled pawns
const Score TRIPLE_PAWN_PENALTY = S(-35, -100);

const Score PAWN_CHAIN_BONUS[FILE_NUM] = {S(0, 0),  S(0, 0),  S(2, 4),
                                          S(4, 10), S(5, 15), S(5, 20),
                                          S(6, 20), S(7, 20)};

const Value CONNECTED_PAWNS_BONUS[FILE_NUM] = {Value(0),  Value(0),  Value(2),
                                               Value(5),  Value(20), Value(40),
                                               Value(80), Value(0)};

const Score BACKWARD_PAWN_PENALTY = S(-30, -100);

const Score ISOLATED_PAWN_PENALTY = S(-20, -80);

const Score KING_SAFETY_BONUS = S(30, 0);
const Score SAFE_KNIGHT = S(10, 2);
const Score CONTROL_CENTER_KNIGHT = S(10, 10);

const Score CONTROL_SPACE[PIECE_KIND_NUM] = {S(20, 30), S(0, 0),   S(10, 5),
                                             S(10, 10), S(10, 20), S(0, 0)};

const Score KING_PROTECTOR_PENALTY[PIECE_KIND_NUM] = {
    S(0, 0), S(-6, -4), S(-5, -3), S(0, 0), S(0, 0), S(0, 0)};

const Score KING_ATTACKER_PENALTY[PIECE_KIND_NUM] = {
    S(0, 0), S(-7, -4), S(-4, -3), S(0, 0), S(0, 0), S(0, 0)};

const Score PAWN_ISLAND_PENALTY = S(-10, -20);

const Score VULNERABLE_QUEEN_PENALTY = S(-30, -15);

const Score WEAK_BACKRANK_PENALTY = S(-50, -50);

const Score WEAK_KING_RAYS_PENALTY = S(-15, -20);

const Score WEAK_KING_DIAGONALS = S(-5, 0);
const Score WEAK_KING_LINES = S(-7, 0);

const Score PAWNS_ON_SAME_COLOR_AS_BISHOP_PENALTY = S(-3, -5);

PositionScorer::PositionScorer() : _pawn_hash_table(), _weight(-1) {}

void PositionScorer::clear()
{
    _pawn_hash_table.clear();
}

Value PositionScorer::combine(const Score& score)
{
    assert(0 <= _weight && _weight <= MAX_PIECE_WEIGTHS);
    return (score.mg * _weight + score.eg * (MAX_PIECE_WEIGTHS - _weight)) /
           MAX_PIECE_WEIGTHS;
}

Value PositionScorer::score(const Position& position)
{
    if (!popcount_more_than_one(position.pieces(WHITE)))
        return endgame::score_endgame<BLACK>(position);
    if (!popcount_more_than_one(position.pieces(BLACK)))
        return endgame::score_endgame<WHITE>(position);

    setup<WHITE>(position);
    setup<BLACK>(position);

    _weight = game_phase_weight(position);

    Score pieces = score_pieces(position);
    Score pawns = score_pawns(position);
    Value value = combine(pawns + pieces);

    return position.color() == WHITE ? value : -value;
}

template <Color side>
void PositionScorer::setup(const Position& position)
{
    const Square ownKing = position.piece_position(make_piece(side, KING), 0);
    const Square opponentsKing =
        position.piece_position(make_piece(!side, KING), 0);

    _attacked_by_bb[side][PAWN] =
        pawn_attacks<side>(position.pieces(make_piece(side, PAWN)));

    _attacked_by_bb[side][KNIGHT] = 0ULL;
    for (int i = 0; i < position.number_of_pieces(make_piece(side, KNIGHT));
         ++i)
    {
        Square sq = position.piece_position(make_piece(side, KNIGHT), i);
        _attacked_by_bb[side][KNIGHT] |= KNIGHT_MASK[sq];
    }

    _attacked_by_bb[side][BISHOP] = 0ULL;
    for (int i = 0; i < position.number_of_pieces(make_piece(side, BISHOP));
         ++i)
    {
        Square sq = position.piece_position(make_piece(side, BISHOP), i);
        // remove own bishops and queens from blockers (x-ray attack)
        Bitboard blockers =
            position.pieces() & (~position.pieces(side, BISHOP, QUEEN));
        _attacked_by_bb[side][BISHOP] |= slider_attack<BISHOP>(sq, blockers);
    }

    _attacked_by_bb[side][ROOK] = 0ULL;
    for (int i = 0; i < position.number_of_pieces(make_piece(side, ROOK)); ++i)
    {
        Square sq = position.piece_position(make_piece(side, ROOK), i);
        // remove own rooks and queens from blockers (x-ray attack)
        Bitboard blockers =
            position.pieces() & (~position.pieces(side, ROOK, QUEEN));
        _attacked_by_bb[side][ROOK] |= slider_attack<ROOK>(sq, blockers);
    }

    _attacked_by_bb[side][QUEEN] = 0ULL;
    for (int i = 0; i < position.number_of_pieces(make_piece(side, QUEEN)); ++i)
    {
        Square sq = position.piece_position(make_piece(side, QUEEN), i);
        // remove own bishops and queens from blockers (x-ray attack)
        Bitboard blockers =
            position.pieces() & (~position.pieces(side, BISHOP, QUEEN));
        _attacked_by_bb[side][QUEEN] |= slider_attack<BISHOP>(sq, blockers);
        // remove own bishops and rooks from blockers (x-ray attack)
        blockers = position.pieces() & (~position.pieces(side, ROOK, QUEEN));
        _attacked_by_bb[side][QUEEN] |= slider_attack<ROOK>(sq, blockers);
    }

    _attacked_by_bb[side][KING] =
        KING_MASK[position.piece_position(make_piece(side, KING), 0)];

    _attacked_by_piece[side] =
        _attacked_by_bb[side][KNIGHT] | _attacked_by_bb[side][BISHOP] |
        _attacked_by_bb[side][ROOK] | _attacked_by_bb[side][QUEEN];

    _outposts_bb[side] = get_outposts<side>(position);

    _blockers_for_king[side] =
        blockers_for_square<side>(position, ownKing, _snipers_for_king[side]);
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
    const Square ownKing = position.piece_position(make_piece(side, KING), 0);
    const Square opponentsKing =
        position.piece_position(make_piece(!side, KING), 0);

    Score value;

    // knights
    {
        constexpr Piece piece = make_piece(side, KNIGHT);
        Bitboard opponent_pieces =
            position.pieces(!side) & ~position.pieces(!side, PAWN);
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
            value +=
                CONTROL_CENTER_KNIGHT * Value(popcount(attacking & center_bb));

            value +=
                KING_PROTECTOR_PENALTY[KNIGHT] * Value(distance(ownKing, sq));
            value += KING_ATTACKER_PENALTY[KNIGHT] *
                     Value(distance(opponentsKing, sq));

            Bitboard moves = attacking;
            if (square_bb(sq) & _blockers_for_king[side])
            {
                moves &= FULL_LINES[sq][ownKing];
            }
            // cannot move into square with our piece
            moves &= ~position.pieces(side);
            // squares attacked by opponents pawns with nothing valueable there
            moves &= ~(_attacked_by_bb[!side][PAWN] & ~opponent_pieces);
            // attacked by opponents piece and not defended by our pawns
            moves &=
                ~(_attacked_by_piece[!side] & ~_attacked_by_bb[side][PAWN]);
            value += MOBILITY_BONUS[KNIGHT] * Value(popcount(moves));

            if (_outposts_bb[side] & square_bb(sq))
            {
                value += OUTPOST_KNIGHT_BONUS;
            }
        }
    }

    // bishop
    {
        constexpr Piece piece = make_piece(side, BISHOP);
        Bitboard opponent_pieces =
            position.pieces(!side) & ~position.pieces(!side, PAWN);
        int no_pieces = position.number_of_pieces(piece);

        value += PIECE_VALUE[BISHOP] * Value(no_pieces);
        value += CONTROL_SPACE[BISHOP] *
                 Value(popcount(_attacked_by_bb[side][BISHOP] &
                                OPPONENT_RANKS[side]));
        for (int i = 0; i < no_pieces; ++i)
        {
            Square sq = position.piece_position(piece, i);
            Bitboard attacking = slider_attack<BISHOP>(sq, position.pieces());

            Bitboard moves = attacking;
            if (square_bb(sq) & _blockers_for_king[side])
            {
                moves &= FULL_LINES[sq][ownKing];
            }
            // cannot move into square with our piece
            moves &= ~position.pieces(side);
            // attacked by opponents pawns
            moves &= ~(_attacked_by_bb[!side][PAWN] & ~opponent_pieces);
            // attacked by opponents piece and not defended by our pawns
            moves &=
                ~(_attacked_by_piece[!side] & ~_attacked_by_bb[side][PAWN]);
            value += MOBILITY_BONUS[BISHOP] * Value(popcount(moves));

            value +=
                KING_PROTECTOR_PENALTY[BISHOP] * Value(distance(ownKing, sq));
            value += KING_ATTACKER_PENALTY[BISHOP] *
                     Value(distance(opponentsKing, sq));

            value += PAWNS_ON_SAME_COLOR_AS_BISHOP_PENALTY *
                     Value(popcount(position.pieces(side, PAWN) &
                                    color_squares[sq_color(sq)]));

            if (_outposts_bb[side] & square_bb(sq))
            {
                value += OUTPOST_BISHOP_BONUS;
            }
        }
        if ((position.pieces(side, BISHOP) & white_squares_bb) &&
            (position.pieces(side, BISHOP) & black_squares_bb))
            value += BISHOP_PAIR_BONUS;
    }

    // rook
    {
        constexpr Piece piece = make_piece(side, ROOK);
        Bitboard opponent_pieces =
            position.pieces(!side) & ~position.pieces(!side, PAWN);
        int no_pieces = position.number_of_pieces(piece);

        value += PIECE_VALUE[ROOK] * Value(no_pieces);
        value +=
            CONTROL_SPACE[ROOK] *
            Value(popcount(_attacked_by_bb[side][ROOK] & OPPONENT_RANKS[side]));
        for (int i = 0; i < no_pieces; ++i)
        {
            Square sq = position.piece_position(piece, i);

            Bitboard file_bb = FILES_BB[file(sq)];
            if (!(file_bb & position.pieces(PAWN)))
                value += ROOK_OPEN_FILE_BONUS;
            if (!(file_bb & position.pieces(side, PAWN)) &&
                file_bb & position.pieces(!side, PAWN))
                value += ROOK_SEMIOPEN_FILE_BONUS;

            if (popcount_more_than_one(file_bb & position.pieces(piece)))
                value += CONNECTED_ROOKS_BONUS;

            Bitboard attacking = slider_attack<ROOK>(sq, position.pieces());

            Bitboard moves = attacking;
            if (square_bb(sq) & _blockers_for_king[side])
            {
                moves &= FULL_LINES[sq][ownKing];
            }
            // cannot move into square with our piece
            moves &= ~position.pieces(side);
            // attacked by opponents pawns
            moves &= ~(_attacked_by_bb[!side][PAWN] & ~opponent_pieces);
            // attacked by opponents piece and not defended by our pawns
            moves &=
                ~(_attacked_by_piece[!side] & ~_attacked_by_bb[side][PAWN]);
            value += MOBILITY_BONUS[ROOK] * Value(popcount(moves));
        }
    }

    // queen
    {
        constexpr Piece piece = make_piece(side, QUEEN);
        Bitboard opponent_pieces =
            position.pieces(!side) & ~position.pieces(!side, PAWN);
        int no_pieces = position.number_of_pieces(piece);

        value += PIECE_VALUE[QUEEN] * Value(no_pieces);
        value +=
            CONTROL_SPACE[QUEEN] * Value(popcount(_attacked_by_bb[side][QUEEN] &
                                                  OPPONENT_RANKS[side]));
        for (int i = 0; i < no_pieces; ++i)
        {
            Square sq = position.piece_position(piece, i);

            Bitboard snipers =
                (pseudoattacks<BISHOP>(sq) & position.pieces(!side, BISHOP)) |
                (pseudoattacks<ROOK>(sq) & position.pieces(!side, ROOK));
            Bitboard rest = position.pieces() & ~(snipers | square_bb(sq));

            while (snipers)
            {
                Square sniper_sq = Square(pop_lsb(&snipers));
                Bitboard blockers = LINES[sq][sniper_sq] & rest;
                // check if there is only one blocker between the queen and the
                // sniper
                if (blockers && !popcount_more_than_one(blockers))
                {
                    value += VULNERABLE_QUEEN_PENALTY;
                    break;
                }
            }

            Bitboard attacking = slider_attack<QUEEN>(sq, position.pieces());
            Bitboard moves = attacking;
            if (square_bb(sq) & _blockers_for_king[side])
            {
                moves &= FULL_LINES[sq][ownKing];
            }
            // cannot move into square with our piece
            moves &= ~position.pieces(side);
            // attacked by opponents pawns
            moves &= ~(_attacked_by_bb[!side][PAWN] & ~opponent_pieces);
            // attacked by opponents piece and not defended by our pawns
            moves &=
                ~(_attacked_by_piece[!side] & ~_attacked_by_bb[side][PAWN]);
            value += MOBILITY_BONUS[QUEEN] * Value(popcount(moves));
        }
    }

    value += score_king_safety<side>(position);

    return value;
}

template <Color side>
Score PositionScorer::score_king_safety(const Position& position)
{
    const Square ownKing = position.piece_position(make_piece(side, KING), 0);
    const Square opponentsKing =
        position.piece_position(make_piece(!side, KING), 0);

    const Rank first_rank = side == WHITE ? RANK_1 : RANK_8;
    const Rank second_rank = side == WHITE ? RANK_2 : RANK_7;

    Score value;

    Bitboard king_area = KING_MASK[ownKing] | square_bb(ownKing);

    Bitboard pawns_in_king_area = king_area & position.pieces(side, PAWN);

    value += Value(popcount(pawns_in_king_area)) * KING_SAFETY_BONUS;

    if (rank(ownKing) == first_rank && position.pieces(!side, ROOK, QUEEN))
    {
        // check for backrank weakness
        Bitboard backrank_area = king_area & RANKS_BB[second_rank];
        Bitboard blocked = position.pieces(side) | _attacked_by_piece[!side] |
                           _attacked_by_bb[!side][PAWN] |
                           KING_MASK[opponentsKing];
        if ((backrank_area & blocked) == backrank_area)
            value += WEAK_BACKRANK_PENALTY;
    }

    value += WEAK_KING_RAYS_PENALTY *
             Value(popcount(_blockers_for_king[side] &
                            ~(position.pieces(side, PAWN))));

    Bitboard possible_bishop_check =
        slider_attack<BISHOP>(ownKing, position.pieces());
    if (position.pieces(!side, QUEEN) ||
        (position.pieces(!side, BISHOP) & color_squares[sq_color(ownKing)]))
        value += WEAK_KING_DIAGONALS * Value(popcount(possible_bishop_check));
    Bitboard possible_rook_check =
        slider_attack<ROOK>(ownKing, position.pieces());
    if (position.pieces(!side, QUEEN) || position.pieces(!side, ROOK))
        value += WEAK_KING_LINES * Value(popcount(possible_rook_check));

    return value;
};

Score PositionScorer::score_pawns(const Position& position)
{
    std::pair<Value, Value> p;
    Score value;

    // check in hash table
    if (!_pawn_hash_table.get(position, p))
    {
        value = score_pawns_for_side<WHITE>(position) -
                score_pawns_for_side<BLACK>(position);
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
    constexpr Direction up_dir = side == WHITE ? NORTH : SOUTH;
    constexpr Direction down_dir = static_cast<Direction>(-up_dir);
    constexpr int up = side == WHITE ? 1 : -1;

    Score value;
    int num_pawns = position.number_of_pieces(pawn);
    value += PIECE_VALUE[PAWN] * Value(num_pawns);

    Bitboard ourPawns = position.pieces(side, PAWN);
    Bitboard theirPawns = position.pieces(!side, PAWN);

    for (int i = 0; i < num_pawns; ++i)
    {
        Square sq = position.piece_position(pawn, i);

        Rank r = rank(sq);
        File f = file(sq);
        Rank rel_rank = relative_rank<side>(r);

        Bitboard neighbours = ourPawns & NEIGHBOUR_FILES_BB[f];
        Bitboard phalanx = neighbours & RANKS_BB[r];
        Bitboard support = neighbours & RANKS_BB[r - up];
        Bitboard lever = theirPawns & pawn_attacks<side>(square_bb(sq));
        Bitboard leverPush =
            theirPawns & shift<up_dir>(pawn_attacks<side>(square_bb(sq)));
        Bitboard opposed = theirPawns & passed_pawn_bb<side>(sq);
        bool blocked =
            static_cast<bool>(theirPawns & shift<up_dir>(square_bb(sq)));
        bool doubled =
            static_cast<bool>(ourPawns & shift<down_dir>(square_bb(sq)));
        bool backward =
            static_cast<bool>(neighbours &
                              passed_pawn_bb<!side>(Square(sq + 8 * up))) &&
            (blocked || leverPush);

        bool passed = !opposed || !(opposed ^ lever) ||
                      (!(opposed ^ leverPush) &&
                       popcount(phalanx) >= popcount(leverPush));

        if (doubled) value += DOUBLE_PAWN_PENALTY;

        if (support | phalanx)
        {
            value += Score(CONNECTED_PAWNS_BONUS[rel_rank] *
                           (1 + static_cast<int>(bool(phalanx)) -
                            static_cast<int>(bool(opposed))));
            value += Score(10 * popcount(support));
        }
        else if (!neighbours)
            value += ISOLATED_PAWN_PENALTY;
        else if (backward)
            value += BACKWARD_PAWN_PENALTY;

        if (passed) value += PASSED_PAWN_BONUS * Value(rel_rank);
    }

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

template <Color side>
Bitboard PositionScorer::blockers_for_square(const Position& position,
                                             Square sq, Bitboard& snipers)
{
    snipers = no_squares_bb;
    Bitboard blockers = no_squares_bb;

    Bitboard possible_snipers =
        (pseudoattacks<BISHOP>(sq) & position.pieces(!side, BISHOP, QUEEN)) |
        (pseudoattacks<ROOK>(sq) & position.pieces(!side, ROOK, QUEEN));

    Bitboard rest = position.pieces() & ~(possible_snipers | square_bb(sq));

    while (possible_snipers)
    {
        Square sniper_sq = Square(pop_lsb(&possible_snipers));

        Bitboard b = LINES[sq][sniper_sq] & rest;
        if (b && !popcount_more_than_one(b))
        {
            snipers |= square_bb(sq);
            blockers |= b;
        }
    }

    return blockers;
}

}  // namespace engine
