#include "movegen.h"
#include "position.h"

namespace engine
{

Move MOVE_LIST[MAX_DEPTH][MAX_MOVES];
Pin PINS[MAX_PINS];

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
    Bitboard bb = 0ULL;
    bb |= attack_in_ray(sq, RAY_NW, blockers);
    bb |= attack_in_ray(sq, RAY_NE, blockers);
    bb |= attack_in_ray(sq, RAY_SW, blockers);
    bb |= attack_in_ray(sq, RAY_SE, blockers);
    return bb;
}

template <>
Bitboard slider_attack<ROOK>(Square sq, Bitboard blockers)
{
    Bitboard bb = 0ULL;
    bb |= attack_in_ray(sq, RAY_N, blockers);
    bb |= attack_in_ray(sq, RAY_S, blockers);
    bb |= attack_in_ray(sq, RAY_W, blockers);
    bb |= attack_in_ray(sq, RAY_E, blockers);
    return bb;
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
    bb |= KNIGHT_MOVES[king_sq] & pieces_bb(pos, opponent, KNIGHT);
    bb |= slider_attack<BISHOP>(king_sq, pieces_bb(pos))
            & pieces_bb(pos, opponent, BISHOP);
    bb |= slider_attack<ROOK>(king_sq, pieces_bb(pos))
            & pieces_bb(pos, opponent, ROOK);
    bb |= slider_attack<QUEEN>(king_sq, pieces_bb(pos))
            & pieces_bb(pos, opponent, QUEEN);

    return bb;
}

template <Color side>
Bitboard attacked_squares(const Position& pos)
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
        bb |= KNIGHT_MOVES[pos.piece_position[OPPONENT_KNIGHT][i]];

    Bitboard blockers = pieces_bb(pos) ^ square_bb(king_sq);
    for (int i = 0 ; i < pos.piece_count[OPPONENT_BISHOP]; ++i)
        bb |= slider_attack<BISHOP>(pos.piece_position[OPPONENT_BISHOP][i], blockers);
    for (int i = 0 ; i < pos.piece_count[OPPONENT_ROOK]; ++i)
        bb |= slider_attack<ROOK>(pos.piece_position[OPPONENT_ROOK][i], blockers);
    for (int i = 0 ; i < pos.piece_count[OPPONENT_QUEEN]; ++i)
        bb |= slider_attack<QUEEN>(pos.piece_position[OPPONENT_QUEEN][i], blockers);

    bb |= KING_MOVES[pos.piece_position[OPPONENT_KING][0]];

    return bb;
}

Move create_move(Square from, Square to)
{
    return Move{from, to, NO_PIECE_KIND, NO_PIECE_KIND, NO_CASTLING, NO_CASTLING, NO_SQUARE, false};
}

template <PieceKind piece>
Move create_promotion(Square from, Square to)
{
    return Move{from, to, NO_PIECE_KIND, piece, NO_CASTLING, NO_CASTLING, NO_SQUARE, false};
}

Move create_castling(Castling castling)
{
    return Move{NO_SQUARE, NO_SQUARE, NO_PIECE_KIND, NO_PIECE_KIND, castling, NO_CASTLING, NO_SQUARE, false};
}

Move create_enpassant(Square from, Square enpassant_square)
{
    return Move{from, enpassant_square, NO_PIECE_KIND, NO_PIECE_KIND, NO_CASTLING, NO_CASTLING, NO_SQUARE, true};
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

    //// pawns not on 7
    // captures to the right
    Bitboard bb = shift<UPRIGHT>(pawnsNotOn7) & capture_mask;
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

    //// pawns on 7
    // captures to the right
    bb = shift<UPRIGHT>(pawnsOn7) & capture_mask;
    FOR_EACH_BIT(bb,
            *list++ = create_promotion<QUEEN >(Square(sq - upright), sq);
            *list++ = create_promotion<ROOK  >(Square(sq - upright), sq);
            *list++ = create_promotion<BISHOP>(Square(sq - upright), sq);
            *list++ = create_promotion<KNIGHT>(Square(sq - upright), sq);
            )

    // captures to the left
    bb = shift<UPLEFT>(pawnsOn7) & capture_mask;
    FOR_EACH_BIT(bb,
            *list++ = create_promotion<QUEEN >(Square(sq - upleft), sq);
            *list++ = create_promotion<ROOK  >(Square(sq - upleft), sq);
            *list++ = create_promotion<BISHOP>(Square(sq - upleft), sq);
            *list++ = create_promotion<KNIGHT>(Square(sq - upleft), sq);
            )

    // moves
    bb = shift<UP>(pawnsOn7) & push_mask & empty;
    FOR_EACH_BIT(bb,
            *list++ = create_promotion<QUEEN >(Square(sq - up), sq);
            *list++ = create_promotion<ROOK  >(Square(sq - up), sq);
            *list++ = create_promotion<BISHOP>(Square(sq - up), sq);
            *list++ = create_promotion<KNIGHT>(Square(sq - up), sq);
            )

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
        /* Bitboard rank_bb = pieces_bb(pos) & RANKS_BB[rank(captured_square)]; */
        /* if (popcount(rank_bb) >= 4) */
        /* { */
            Bitboard blockers = pieces_bb(pos) ^ (square_bb(captured_square) | attacking_bb);
            /* print_bb(blockers); */
            Square king_square = pos.piece_position[make_piece(side, KING)][0];
            Bitboard attacked_bb = attack_in_line(king_square, RAY_E, blockers);
            /* print_bb(attacked_bb); */

            if (attacked_bb & (pieces_bb(pos, !side, ROOK) | pieces_bb(pos, !side, QUEEN)))
                return list;
        /* } */
    }

    if (right_bb)
        *list++ = create_enpassant(Square(enpassant_square - upright), enpassant_square);
    if (left_bb)
        *list++ = create_enpassant(Square(enpassant_square - upleft), enpassant_square);

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
                ? KNIGHT_MOVES[from]
                : slider_attack<piece>(from, pieces_bb(pos));
    bb &= target;

    FOR_EACH_BIT(bb, *list++ = create_move(from, sq))

    return list;
}

Move* generate_king_moves(Square from, Bitboard not_allowed, Move* list)
{
    Bitboard bb = KING_MOVES[from] & ~not_allowed;
    FOR_EACH_BIT(bb, *list++ = create_move(from, sq));
    return list;
}

template <Color side>
Pin* generate_pins(const Position& pos, Pin* list, Bitboard* pinned_bb)
{
    const Piece C_KING = side == WHITE ? W_KING : B_KING;
    const Piece O_BISHOP = side == WHITE ? B_KNIGHT : W_KNIGHT;
    const Piece O_ROOK = side == WHITE ? B_ROOK : W_ROOK;
    const Piece O_QUEEN = side == WHITE ? B_QUEEN : W_QUEEN;

    Square king_sq = pos.piece_position[C_KING][0];

    RayDirection rays[] = {RAY_NW, RAY_N, RAY_NE, RAY_E, RAY_SE, RAY_S, RAY_SW, RAY_W};

    *pinned_bb = 0ULL;

    Bitboard blockers = pieces_bb(pos);
    for (int dir = 0; dir < 8; ++dir)
    {
        Bitboard attacked = 0ULL;
        for (int i = 0; i < pos.piece_count[O_QUEEN]; ++i)
            attacked |= attack_in_ray(pos.piece_position[O_QUEEN][i], rays[dir], blockers);

        if (dir & 1)
            for (int i = 0; i < pos.piece_count[O_ROOK]; ++i)
                attacked |= attack_in_ray(pos.piece_position[O_ROOK][i], rays[dir], blockers);
        else
            for (int i = 0; i < pos.piece_count[O_BISHOP]; ++i)
                attacked |= attack_in_ray(pos.piece_position[O_BISHOP][i], rays[dir], blockers);

        Bitboard king_side = attack_in_ray(king_sq, rays[(dir + 4) & 7], blockers);
        Bitboard pinned = attacked & king_side & pieces_bb(pos, side);

        if (pinned)
        {
            assert(popcount(pinned) == 1);
            Square pinned_sq = Square(lsb(pinned));
            PieceKind pinned_piece = PieceKind((pos.board[pinned_sq] - 1) % 6 + 1);

            *list++ = Pin{pinned_sq, pinned_piece, RayDirection(dir)};
            *pinned_bb |= square_bb(pinned_sq);
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


    Bitboard bb = 0ULL;
    switch (ray & 3)
    {
    case 0:
        bb = shift<UPLEFT>(square_bb(from)) & pieces_bb(pos, Color(1 - side));
        break;
    case 1:
        bb = shift<UP>(square_bb(from)) & ~pieces_bb(pos);
        break;
    case 2:
        bb = shift<UPRIGHT>(square_bb(from)) & pieces_bb(pos, Color(1 - side));
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
Move* generate_legal_moves(const Position& pos, Move* list)
{
    const Piece C_KING = side == WHITE ? W_KING : B_KING;
    Bitboard checkers_bb = checkers<side>(pos);

    Bitboard pinned = 0ULL;
    Pin* pins_start = PINS;
    Pin* pins_end = generate_pins<side>(pos, pins_start, &pinned);

    Bitboard push_mask;
    Bitboard capture_mask;
    Bitboard attacked = attacked_squares<side>(pos);

    Square king_sq = pos.piece_position[C_KING][0];
    list = generate_king_moves(king_sq, attacked | pieces_bb(pos, side), list);

    if (!checkers_bb)
    {
        push_mask = ~pieces_bb(pos);
        capture_mask = pieces_bb(pos, !side);

        for (Pin* iter = pins_start; iter != pins_end; ++iter)
            list = generate_pinned_piece_moves<side>(iter->square, iter->piece_kind, iter->direction, pos, push_mask | capture_mask, list);

        if (side == WHITE)
        {
            if ((pos.castling_rights & W_OO) && !((attacked | pieces_bb(pos)) & CASTLING_PATHS[W_OO]))
                *list++ = create_castling(W_OO);
        }
        else
        {
            if ((pos.castling_rights & B_OO) && !((attacked | pieces_bb(pos)) & CASTLING_PATHS[B_OO]))
                *list++ = create_castling(B_OO);
        }

        if (side == WHITE)
        {
            if ((pos.castling_rights & W_OOO) && !((attacked | pieces_bb(pos)) & CASTLING_PATHS[W_OOO]))
                *list++ = create_castling(W_OOO);
        }
        else
        {
            if ((pos.castling_rights & B_OOO) && !((attacked | pieces_bb(pos)) & CASTLING_PATHS[B_OOO]))
                *list++ = create_castling(B_OOO);
        }
    }
    else
    {
        if (popcount(checkers_bb) > 1) return list;

        capture_mask = checkers_bb;
        Square checker_square = Square(lsb(checkers_bb));

        if (is_piece_slider(pos.board[checker_square]))
            push_mask = LINES[king_sq][checker_square] ^ square_bb(king_sq) ^ square_bb(checker_square);
        else
            push_mask = 0ULL;

    }

    Bitboard target = push_mask | capture_mask;

    Bitboard not_pinned_pawns = pieces_bb(pos, side, PAWN) & ~pinned;
    list = generate_pawn_moves<side>(not_pinned_pawns, ~pieces_bb(pos), push_mask, capture_mask, list);

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

    return list;
}

}  // namespace

Move* generate_moves(const Position& position, Move* list)
{
    return position.current_side == WHITE ?
            generate_legal_moves<WHITE>(position, list) :
            generate_legal_moves<BLACK>(position, list);
}

};
