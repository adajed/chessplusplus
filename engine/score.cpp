#include "position.h"
#include "score.h"
#include "types.h"

namespace engine
{

Square flip(Square square)
{
    return make_square(RANK_8 - rank(square), file(square));
}

PositionScorer::PositionScorer(const Weights& weights)
    : weights(weights)
{
}

int64_t PositionScorer::score(const Position& position) const
{
    int64_t value = score<WHITE>(position) - score<BLACK>(position);
    return position.current_side == WHITE ? value : -value;
}

template <Color side>
int64_t PositionScorer::score(const Position& position) const
{
    int64_t value = 0LL;

    GamePhase phase = get_game_phase(position);

    Move* begin = QUIESCENCE_MOVE_LIST[0];
    Move* end = generate_moves(position, begin);
    uint32_t mobility[PIECE_KIND_NUM];
    for (PieceKind piecekind = PAWN; piecekind != KING; ++piecekind)
        mobility[piecekind] = 0;

    for (Move* it = begin; it != end; ++it)
    {
        PieceKind piecekind = make_piece_kind(position.board[from(*it)]);
        mobility[piecekind]++;
    }

    for (PieceKind piecekind = PAWN; piecekind != KING; ++piecekind)
    {
        Piece piece = make_piece(side, piecekind);
        int num_pieces = position.piece_count[piece];
        value += weights.piece_values[phase][piecekind] * num_pieces;
        value += mobility[piecekind] * weights.mobility_bonus[phase][piecekind];

        for (int i = 0; i < num_pieces; ++i)
        {
            Square square = position.piece_position[piece][i];
            square = side == WHITE ? square : flip(square);
            value += weights.piece_positional_values[phase][piecekind][square];
        }
    }

    value += mobility[KING] * weights.mobility_bonus[phase][KING];
    Square square = position.piece_position[make_piece(side, KING)][0];
    square = side == WHITE ? square : flip(square);
    value += weights.piece_positional_values[phase][KING][square];

    constexpr Piece ROOK_PIECE = make_piece(side, ROOK);
    for (int i = 0; i < position.piece_count[ROOK_PIECE]; ++i)
    {
        Square square = position.piece_position[ROOK_PIECE][i];
        Bitboard file_bb = FILES_BB[file(square)];

        if (!(file_bb & pieces_bb(position, PAWN)))
            value += weights.rook_open_file_bonus[phase];
        if (!(file_bb & pieces_bb(position, side, PAWN)) &&
                file_bb & pieces_bb(position, !side, PAWN))
            value += weights.rook_semiopen_file_bonus[phase];
    }

    if ((pieces_bb(position, side, BISHOP) & white_squares_bb) &&
            (pieces_bb(position, side, BISHOP) & black_squares_bb))
        value += weights.bishop_pair_bonus[phase];

    constexpr Piece PAWN_PIECE = make_piece(side, PAWN);

    Bitboard pawns = pieces_bb(position, side, PAWN);

    for (File file = FILE_A; file <= FILE_H; ++file)
        if (popcount_more_than_one(pawns & FILES_BB[file]))
             value += weights.doubled_pawn_penalty[phase];

    for (int i = 0; i < position.piece_count[PAWN_PIECE]; ++i)
    {
        Square square = position.piece_position[PAWN_PIECE][i];
        if (NEIGHBOUR_FILES_BB[file(square)] & pawns)
            value += weights.isolated_pawn_penalty[phase];

        if (!(passed_pawn_bb(side, square) & pieces_bb(position, !side, PAWN)))
            value += weights.passed_pawn_bonus[phase];
    }


    return value;
}

}
