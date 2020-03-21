#include "position.h"
#include "score.h"
#include "types.h"

namespace engine
{

Square flip(Square square)
{
    return make_square(RANK_8 - rank(square), file(square));
}

PositionScorer::PositionScorer(Weights& weights)
    : weights(weights)
{
}

int64_t PositionScorer::score(const Position& position)
{
    for (Color side : {WHITE, BLACK})
        for (PieceKind piecekind : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING})
            move_count[side][piecekind] = 0;

    for (Color side : {WHITE, BLACK})
    {
        Move* begin = move_list;
        Move* end = generate_moves(position, side, begin);
        for (Move* it = begin; it != end; ++it)
        {
            PieceKind piecekind = make_piece_kind(position.piece_at(from(*it)));
            move_count[side][piecekind]++;
        }
    }

    int64_t value = score_side<WHITE>(position) - score_side<BLACK>(position);
    return position.side_to_move() == WHITE ? value : -value;
}

template <Color side>
int64_t PositionScorer::score_side(const Position& position)
{
    int64_t phase_weight = game_phase_weight(position);
    int64_t mg = score<side, MIDDLE_GAME>(position);
    int64_t eg = score<side, END_GAME>(position);

    return ((mg * phase_weight) + (eg * (weights.max_weight - phase_weight))) / weights.max_weight;
}

template <Color side, GamePhase phase>
int64_t PositionScorer::score(const Position& position)
{
    int64_t value = 0LL;

    for (PieceKind piecekind : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN})
    {
        Piece piece = make_piece(side, piecekind);
        int num_pieces = position.number_of_pieces(piece);
        value += weights.piece_values[phase][piecekind] * num_pieces;
        value += weights.mobility_bonus[phase][piecekind] * move_count[side][piecekind];

        for (int i = 0; i < num_pieces; ++i)
        {
            Square square = position.piece_position(piece, i);
            square = side == WHITE ? square : flip(square);
            value += weights.piece_positional_values[phase][piecekind][square];
        }
    }

    Square square = position.piece_position(make_piece(side, KING), 0);
    square = side == WHITE ? square : flip(square);
    value += weights.piece_positional_values[phase][KING][square];

    constexpr Piece ROOK_PIECE = make_piece(side, ROOK);
    for (int i = 0; i < position.number_of_pieces(ROOK_PIECE); ++i)
    {
        Square square = position.piece_position(ROOK_PIECE, i);
        Bitboard file_bb = FILES_BB[file(square)];

        if (!(file_bb & position.pieces(PAWN)))
            value += weights.rook_open_file_bonus[phase];
        if (!(file_bb & position.pieces(side, PAWN)) &&
                file_bb & position.pieces(!side, PAWN))
            value += weights.rook_semiopen_file_bonus[phase];
    }

    if (position.number_of_pieces(make_piece(side, ROOK)) == 2)
    {
        Square rook1 = position.piece_position(make_piece(side, ROOK), 0);
        Square rook2 = position.piece_position(make_piece(side, ROOK), 1);

        if (file(rook1) == file(rook2) || rank(rook1) == rank(rook2))
        {
            Bitboard path = LINES[rook1][rook2] & ~square_bb(rook1) & ~square_bb(rook2);
            if (!(position.pieces() & path))
                value += weights.connected_rook_bonus[phase];
        }
    }

    if ((position.pieces(side, BISHOP) & white_squares_bb) &&
            (position.pieces(side, BISHOP) & black_squares_bb))
        value += weights.bishop_pair_bonus[phase];

    constexpr Piece PAWN_PIECE = make_piece(side, PAWN);

    Bitboard pawns = position.pieces(side, PAWN);

    for (File file = FILE_A; file <= FILE_H; ++file)
        if (popcount_more_than_one(pawns & FILES_BB[file]))
             value += weights.doubled_pawn_penalty[phase];

    for (int i = 0; i < position.number_of_pieces(PAWN_PIECE); ++i)
    {
        Square square = position.piece_position(PAWN_PIECE, i);
        if (!(NEIGHBOUR_FILES_BB[file(square)] & pawns))
            value += weights.isolated_pawn_penalty[phase];

        if (!(passed_pawn_bb(side, square) & position.pieces(!side, PAWN)))
            value += weights.passed_pawn_bonus[phase];
    }

    return value;
}

int64_t PositionScorer::game_phase_weight(const Position& position)
{
    int64_t weight = 0;
    weight += position.number_of_pieces(W_KNIGHT) * weights.piece_weights[KNIGHT];
    weight += position.number_of_pieces(W_BISHOP) * weights.piece_weights[BISHOP];
    weight += position.number_of_pieces(W_ROOK) * weights.piece_weights[ROOK];
    weight += position.number_of_pieces(W_QUEEN) * weights.piece_weights[QUEEN];
    weight += position.number_of_pieces(B_KNIGHT) * weights.piece_weights[KNIGHT];
    weight += position.number_of_pieces(B_BISHOP) * weights.piece_weights[BISHOP];
    weight += position.number_of_pieces(B_ROOK) * weights.piece_weights[ROOK];
    weight += position.number_of_pieces(B_QUEEN) * weights.piece_weights[QUEEN];

    return std::min(weight, weights.max_weight);
}

}
