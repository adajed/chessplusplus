#include "movegen.h"
#include "position.h"

namespace engine
{

Move MOVE_LIST[NUM_THREADS][MAX_DEPTH][MAX_MOVES];
Pin PINS[NUM_THREADS][MAX_PINS];

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

namespace
{

Bitboard attack_in_ray(Square sq, RayDirection ray, Bitboard blockers)
{
    Bitboard masked_blockers = blockers & RAYS[ray][sq];
    if (!masked_blockers)
        return RAYS[ray][sq];
    Square blocker;
    if (RAY_NW <= ray && ray < RAY_SE)
        blocker = Square(lsb(masked_blockers));
    else
        blocker = Square(msb(masked_blockers));

    return RAYS[ray][sq] & ~RAYS[ray][blocker];
}

Bitboard attack_in_line(Square sq, RayDirection ray, Bitboard blockers)
{
    return attack_in_ray(sq, ray, blockers) | attack_in_ray(sq, opposite_ray(ray), blockers);
}

template <PieceKind piece>
Bitboard slider_attack(Square sq, Bitboard blockers)
{
    assert(false);
    return 0ULL;
}

template <>
Bitboard slider_attack<BISHOP>(Square sq, Bitboard blockers)
{
    blockers &= BISHOP_MASK[sq];
    uint64_t key = (blockers * BISHOP_MAGICS[sq]) >> (64 - BISHOP_INDEX_BITS[sq]);
    return BISHOP_TABLE[sq][key];
}

template <>
Bitboard slider_attack<ROOK>(Square sq, Bitboard blockers)
{
    blockers &= ROOK_MASK[sq];
    uint64_t key = (blockers * ROOK_MAGICS[sq]) >> (64 - ROOK_INDEX_BITS[sq]);
    return ROOK_TABLE[sq][key];
}

template <>
Bitboard slider_attack<QUEEN>(Square sq, Bitboard blockers)
{
    return slider_attack<BISHOP>(sq, blockers)
            | slider_attack<ROOK>(sq, blockers);
}


template <Color side>
Bitboard checkers(const Position& pos)
{
    const Color opponent = !side;
    const Piece CURRENT_KING = side == WHITE ? W_KING : B_KING;
    const Direction UPLEFT = side == WHITE ? NORTHWEST : SOUTHEAST;
    const Direction UPRIGHT = side == WHITE ? NORTHEAST : SOUTHWEST;

    Square king_sq = pos.piece_position[CURRENT_KING][0];

    Bitboard bb = 0ULL;

    bb |= (shift<UPLEFT>(square_bb(king_sq)) | shift<UPRIGHT>(square_bb(king_sq)))
            & pieces_bb(pos, opponent, PAWN);
    bb |= KNIGHT_MASK[king_sq] & pieces_bb(pos, opponent, KNIGHT);
    bb |= slider_attack<BISHOP>(king_sq, pieces_bb(pos))
            & pieces_bb(pos, opponent, BISHOP);
    bb |= slider_attack<ROOK>(king_sq, pieces_bb(pos))
            & pieces_bb(pos, opponent, ROOK);
    bb |= slider_attack<QUEEN>(king_sq, pieces_bb(pos))
            & pieces_bb(pos, opponent, QUEEN);

    return bb;
}

template <Color side>
Bitboard forbidden_squares(const Position& pos)
{
    const Color opponent = Color(1 - side);
    const Direction UPLEFT = side == BLACK ? NORTHWEST : SOUTHEAST;
    const Direction UPRIGHT = side == BLACK ? NORTHEAST : SOUTHWEST;

    const Piece OPPONENT_KNIGHT = side == WHITE ? B_KNIGHT : W_KNIGHT;
    const Piece OPPONENT_BISHOP = side == WHITE ? B_BISHOP : W_BISHOP;
    const Piece OPPONENT_ROOK = side == WHITE ? B_ROOK : W_ROOK;
    const Piece OPPONENT_QUEEN = side == WHITE ? B_QUEEN : W_QUEEN;
    const Piece OPPONENT_KING = side == WHITE ? B_KING : W_KING;

    const Square king_sq = side == WHITE
                         ? pos.piece_position[W_KING][0]
                         : pos.piece_position[B_KING][0];

    Bitboard bb = 0ULL;

    Bitboard pawns = pieces_bb(pos, opponent, PAWN);
    bb |= shift<UPLEFT>(pawns) | shift<UPRIGHT>(pawns);

    for (int i = 0; i < pos.piece_count[OPPONENT_KNIGHT]; ++i)
        bb |= KNIGHT_MASK[pos.piece_position[OPPONENT_KNIGHT][i]];

    Bitboard blockers = pieces_bb(pos) ^ square_bb(king_sq);
    for (int i = 0 ; i < pos.piece_count[OPPONENT_BISHOP]; ++i)
        bb |= slider_attack<BISHOP>(pos.piece_position[OPPONENT_BISHOP][i], blockers);
    for (int i = 0 ; i < pos.piece_count[OPPONENT_ROOK]; ++i)
        bb |= slider_attack<ROOK>(pos.piece_position[OPPONENT_ROOK][i], blockers);
    for (int i = 0 ; i < pos.piece_count[OPPONENT_QUEEN]; ++i)
        bb |= slider_attack<QUEEN>(pos.piece_position[OPPONENT_QUEEN][i], blockers);

    bb |= KING_MASK[pos.piece_position[OPPONENT_KING][0]];

    return bb;
}

#define FOR_EACH_BIT(bitboard, code)        \
while (bitboard)                           \
{                                           \
    Square sq = Square(pop_lsb(&bitboard)); \
    code;                                   \
}

// generate moves for not pinned pawns
template <Color side>
Move* generate_pawn_moves(Bitboard pawns, Bitboard empty, Bitboard push_mask, Bitboard capture_mask, Move* list)
{
    const Direction UP      = side == WHITE ? NORTH : SOUTH;
    const Direction UPRIGHT = side == WHITE ? NORTHEAST : SOUTHWEST;
    const Direction UPLEFT  = side == WHITE ? NORTHWEST : SOUTHEAST;
    const Bitboard rank3 = side == WHITE ? rank3_bb : rank6_bb;
    const Bitboard rank7 = side == WHITE ? rank7_bb : rank2_bb;
    const int up      = side == WHITE ? 8 : -8;
    const int upright = side == WHITE ? 9 : -9;
    const int upleft  = side == WHITE ? 7 : -7;

    Bitboard pawnsOn7 = pawns & rank7;
    Bitboard pawnsNotOn7 = pawns & ~rank7;

    //// pawns on 7
    // captures to the right
    Bitboard bb = shift<UPRIGHT>(pawnsOn7) & capture_mask;
    FOR_EACH_BIT(bb,
            *list++ = create_promotion(Square(sq - upright), sq, QUEEN);
            *list++ = create_promotion(Square(sq - upright), sq, ROOK);
            *list++ = create_promotion(Square(sq - upright), sq, BISHOP);
            *list++ = create_promotion(Square(sq - upright), sq, KNIGHT);
            )

    // captures to the left
    bb = shift<UPLEFT>(pawnsOn7) & capture_mask;
    FOR_EACH_BIT(bb,
            *list++ = create_promotion(Square(sq - upleft), sq, QUEEN);
            *list++ = create_promotion(Square(sq - upleft), sq, ROOK);
            *list++ = create_promotion(Square(sq - upleft), sq, BISHOP);
            *list++ = create_promotion(Square(sq - upleft), sq, KNIGHT);
            )

    // moves
    bb = shift<UP>(pawnsOn7) & push_mask & empty;
    FOR_EACH_BIT(bb,
            *list++ = create_promotion(Square(sq - up), sq, QUEEN);
            *list++ = create_promotion(Square(sq - up), sq, ROOK);
            *list++ = create_promotion(Square(sq - up), sq, BISHOP);
            *list++ = create_promotion(Square(sq - up), sq, KNIGHT);
            )

    //// pawns not on 7
    // captures to the right
    bb = shift<UPRIGHT>(pawnsNotOn7) & capture_mask;
    FOR_EACH_BIT(bb, *list++ = create_move(Square(sq - upright), sq))

    // captures to the left
    bb = shift<UPLEFT>(pawnsNotOn7) & capture_mask;
    FOR_EACH_BIT(bb, *list++ = create_move(Square(sq - upleft), sq))

    // moves
    Bitboard pushed_pawns = shift<UP>(pawnsNotOn7) & empty;
    bb = pushed_pawns & push_mask;
    FOR_EACH_BIT(bb, *list++ = create_move(Square(sq - up), sq))
    bb = shift<UP>(pushed_pawns & rank3) & push_mask & empty;
    FOR_EACH_BIT(bb, *list++ = create_move(Square(sq - 2 * up), sq))

    return list;
}

template <Color side>
Move* generate_enpassant(const Position& pos, Bitboard not_pinned_pawns, Bitboard push_mask, Bitboard capture_mask, Square enpassant_square, Move* list)
{
    const Direction UPRIGHT = side == WHITE ? NORTHEAST : SOUTHWEST;
    const Direction UPLEFT  = side == WHITE ? NORTHWEST : SOUTHEAST;
    const int up      = side == WHITE ? 8 : -8;
    const int upright = side == WHITE ? 9 : -9;
    const int upleft  = side == WHITE ? 7 : -7;

    Square captured_square = Square(enpassant_square - up);

    if (!(square_bb(captured_square) & capture_mask || square_bb(enpassant_square) & push_mask))
        return list;

    Bitboard right_bb = shift<UPRIGHT>(not_pinned_pawns) & square_bb(enpassant_square);
    Bitboard left_bb  = shift<UPLEFT >(not_pinned_pawns) & square_bb(enpassant_square);

    if (bool(right_bb) != bool(left_bb))
    {
        Bitboard attacking_bb = 0ULL;
        if (right_bb)
            attacking_bb = square_bb(Square(enpassant_square - upright));
        if (left_bb)
            attacking_bb = square_bb(Square(enpassant_square - upleft));
        Bitboard blockers = pieces_bb(pos) ^ (square_bb(captured_square) | attacking_bb);
        Square king_square = pos.piece_position[make_piece(side, KING)][0];
        Bitboard attacked_bb = attack_in_line(king_square, RAY_E, blockers);

        if (attacked_bb & (pieces_bb(pos, !side, ROOK) | pieces_bb(pos, !side, QUEEN)))
            return list;
    }

    if (right_bb)
        *list++ = create_move(Square(enpassant_square - upright), enpassant_square);
    if (left_bb)
        *list++ = create_move(Square(enpassant_square - upleft), enpassant_square);

    return list;
}

// generates all moves for a given piece,
//  assumes that king is not in check
//  and that piece is not pinned
template <PieceKind piece>
Move* generate_piece_moves(Square from, const Position& pos, Bitboard target, Move* list)
{
    assert(piece != PAWN && piece != KING);

    Bitboard bb = piece == KNIGHT
                ? KNIGHT_MASK[from]
                : slider_attack<piece>(from, pieces_bb(pos));
    bb &= target;

    FOR_EACH_BIT(bb, *list++ = create_move(from, sq))

    return list;
}

Move* generate_king_moves(Square from, Bitboard not_allowed, Move* list)
{
    Bitboard bb = KING_MASK[from] & ~not_allowed;
    FOR_EACH_BIT(bb, *list++ = create_move(from, sq));
    return list;
}

template <Color side>
Pin* generate_pins(const Position& pos, Pin* list, Bitboard* pinned_bb)
{

    const Piece C_KING = side == WHITE ? W_KING : B_KING;
    Square king_sq = pos.piece_position[C_KING][0];

    *pinned_bb = 0ULL;

    Bitboard blockers = pieces_bb(pos);

    for (int dir = 0; dir < 8; ++dir)
    {
        Bitboard masked_ray = RAYS[dir][king_sq] & blockers;

        /* if (popcount(masked_ray) > 1) */
        if (masked_ray && (masked_ray & (masked_ray - 1)))
        {
            Square pinned_sq, attacking_sq;
            if (dir < 4)
            {
                pinned_sq = Square(pop_lsb(&masked_ray));
                attacking_sq = Square(lsb(masked_ray));
            }
            else
            {
                pinned_sq = Square(msb(masked_ray));
                masked_ray &= ~square_bb(pinned_sq);
                attacking_sq = Square(msb(masked_ray));
            }

            if (square_bb(pinned_sq) & pieces_bb(pos, side))
            {
                Bitboard sliders = pieces_bb(pos, !side, QUEEN);
                if (dir & 1)
                    sliders |= pieces_bb(pos, !side, ROOK);
                else
                    sliders |= pieces_bb(pos, !side, BISHOP);

                if (square_bb(attacking_sq) & sliders)
                {
                    *list++ = Pin{pinned_sq, make_piece_kind(pos.board[pinned_sq]), RayDirection(dir)};
                    *pinned_bb |= square_bb(pinned_sq);
                }
            }
        }
    }

    return list;
}

template <Color side>
Move* generate_pinned_pawn_moves(Square from, RayDirection ray, const Position& pos, Bitboard target, Move* list)
{
    const Direction UP = side == WHITE ? NORTH : SOUTH;
    const Direction UPRIGHT = side == WHITE ? NORTHEAST : SOUTHWEST;
    const Direction UPLEFT = side == WHITE ? NORTHWEST : SOUTHEAST;
    const Bitboard rank3_bb = side == WHITE ? RANKS_BB[RANK_3] : RANKS_BB[RANK_6];


    Bitboard bb = 0ULL;
    switch (ray & 3)
    {
    case 0:
        bb = shift<UPLEFT>(square_bb(from)) & pieces_bb(pos, !side);
        break;
    case 1:
        bb = shift<UP>(square_bb(from)) & ~pieces_bb(pos);
        bb |= shift<UP>(bb & rank3_bb) & ~pieces_bb(pos);
        break;
    case 2:
        bb = shift<UPRIGHT>(square_bb(from)) & pieces_bb(pos, !side);
        break;
    }

    FOR_EACH_BIT(bb, *list++ = create_move(from, sq))
    return list;
}

template <Color side>
Move* generate_pinned_piece_moves(Square from, PieceKind piece, RayDirection ray, const Position& pos, Bitboard target, Move* list)
{
    assert(piece != KING);

    // pinned knight cannot move
    if (piece == KNIGHT)
        return list;

    if (piece == PAWN)
        return generate_pinned_pawn_moves<side>(from, ray, pos, target, list);

    if (!allowed_ray(piece, ray))
        return list;

    Bitboard bb = attack_in_line(from, ray, pieces_bb(pos)) & target;
    /* print_bb(bb); */

    FOR_EACH_BIT(bb, *list++ = create_move(from, sq))

    return list;
}

template <Color side>
Move* generate_legal_moves(const Position& pos, int id, Move* list)
{
    const Piece C_KING = side == WHITE ? W_KING : B_KING;
    Bitboard checkers_bb = checkers<side>(pos);

    Bitboard pinned = 0ULL;
    Pin* pins_start = PINS[id];
    Pin* pins_end = generate_pins<side>(pos, pins_start, &pinned);

    Bitboard push_mask;
    Bitboard capture_mask;
    Bitboard attacked = forbidden_squares<side>(pos);

    Square king_sq = pos.piece_position[C_KING][0];

    if (checkers_bb)
    {
        if (popcount(checkers_bb) > 1)
            return generate_king_moves(king_sq, attacked | pieces_bb(pos, side), list);

        capture_mask = checkers_bb;
        Square checker_square = Square(lsb(checkers_bb));

        if (is_piece_slider(pos.board[checker_square]))
            push_mask = LINES[king_sq][checker_square] ^ square_bb(king_sq) ^ square_bb(checker_square);
        else
            push_mask = 0ULL;
    }
    else
    {
        push_mask = ~pieces_bb(pos);
        capture_mask = pieces_bb(pos, !side);
    }

    Bitboard not_pinned_pawns = pieces_bb(pos, side, PAWN) & ~pinned;
    list = generate_pawn_moves<side>(not_pinned_pawns, ~pieces_bb(pos), push_mask, capture_mask, list);

    Bitboard target = capture_mask | push_mask;

    Bitboard not_pinned_knights = pieces_bb(pos, side, KNIGHT) & ~pinned;
    FOR_EACH_BIT(not_pinned_knights, list = generate_piece_moves<KNIGHT>(sq, pos, target, list));

    Bitboard not_pinned_bishops = pieces_bb(pos, side, BISHOP) & ~pinned;
    FOR_EACH_BIT(not_pinned_bishops, list = generate_piece_moves<BISHOP>(sq, pos, target, list));

    Bitboard not_pinned_rooks = pieces_bb(pos, side, ROOK) & ~pinned;
    FOR_EACH_BIT(not_pinned_rooks, list = generate_piece_moves<ROOK>(sq, pos, target, list));

    Bitboard not_pinned_queens = pieces_bb(pos, side, QUEEN) & ~pinned;
    FOR_EACH_BIT(not_pinned_queens, list = generate_piece_moves<QUEEN>(sq, pos, target, list));

    if (pos.enpassant != NO_SQUARE)
        list = generate_enpassant<side>(pos, not_pinned_pawns, push_mask, capture_mask, pos.enpassant, list);

    list = generate_king_moves(king_sq, attacked | pieces_bb(pos, side), list);

    if (!checkers_bb)
    {
        for (Pin* iter = pins_start; iter != pins_end; ++iter)
            list = generate_pinned_piece_moves<side>(iter->square, iter->piece_kind, iter->direction, pos, push_mask | capture_mask, list);

        Bitboard taken_for_castling = attacked | pieces_bb(pos);

        if (side == WHITE)
        {
            if ((pos.castling_rights & W_OO) && !(taken_for_castling & CASTLING_PATHS[W_OO]))
                *list++ = create_castling(KING_CASTLING);
        }
        else
        {
            if ((pos.castling_rights & B_OO) && !(taken_for_castling & CASTLING_PATHS[B_OO]))
                *list++ = create_castling(KING_CASTLING);
        }
        if (side == WHITE)
        {
            if ((pos.castling_rights & W_OOO) &&
                    !(taken_for_castling & CASTLING_PATHS[W_OOO]) &&
                    !(QUEEN_CASTLING_BLOCK[WHITE] & pieces_bb(pos)))
                *list++ = create_castling(QUEEN_CASTLING);
        }
        else
        {
            if ((pos.castling_rights & B_OOO) &&
                    !(taken_for_castling & CASTLING_PATHS[B_OOO]) &&
                    !(QUEEN_CASTLING_BLOCK[BLACK] & pieces_bb(pos)))
                *list++ = create_castling(QUEEN_CASTLING);
        }
    }

    return list;
}

}  // namespace

Move* generate_moves(const Position& position, int id, Move* list)
{
    return position.current_side == WHITE ?
            generate_legal_moves<WHITE>(position, id, list) :
            generate_legal_moves<BLACK>(position, id, list);
}

Bitboard attacked_squares(const Position& position)
{
    const Color side = position.current_side;
    const Color opponent = !side;
    const Direction UPLEFT  = side == BLACK ? NORTHWEST : SOUTHEAST;
    const Direction UPRIGHT = side == BLACK ? NORTHEAST : SOUTHWEST;

    const Piece OPPONENT_KNIGHT = side == WHITE ? B_KNIGHT : W_KNIGHT;
    const Piece OPPONENT_BISHOP = side == WHITE ? B_BISHOP : W_BISHOP;
    const Piece OPPONENT_ROOK = side == WHITE ? B_ROOK : W_ROOK;
    const Piece OPPONENT_QUEEN = side == WHITE ? B_QUEEN : W_QUEEN;
    const Piece OPPONENT_KING = side == WHITE ? B_KING : W_KING;

    Bitboard bb = 0ULL;

    Bitboard pawns = pieces_bb(position, opponent, PAWN);
    bb |= shift(pawns, UPLEFT) | shift(pawns, UPRIGHT);

    for (int i = 0; i < position.piece_count[OPPONENT_KNIGHT]; ++i)
        bb |= KNIGHT_MASK[position.piece_position[OPPONENT_KNIGHT][i]];

    Bitboard blockers = pieces_bb(position);
    for (int i = 0 ; i < position.piece_count[OPPONENT_BISHOP]; ++i)
        bb |= slider_attack<BISHOP>(position.piece_position[OPPONENT_BISHOP][i], blockers);
    for (int i = 0 ; i < position.piece_count[OPPONENT_ROOK]; ++i)
        bb |= slider_attack<ROOK>(position.piece_position[OPPONENT_ROOK][i], blockers);
    for (int i = 0 ; i < position.piece_count[OPPONENT_QUEEN]; ++i)
        bb |= slider_attack<QUEEN>(position.piece_position[OPPONENT_QUEEN][i], blockers);

    bb |= KING_MASK[position.piece_position[OPPONENT_KING][0]];

    return bb;
}

};
