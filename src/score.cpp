#include "position.h"
#include "score.h"
#include "types.h"

namespace engine
{

int64_t PIECE_WEIGHTS[PIECE_KIND_NUM] = {0, 0, 2, 2, 4, 8, 0};
int64_t MAX_WEIGHT = 2 * (2 * PIECE_WEIGHTS[KNIGHT] +
                          2 * PIECE_WEIGHTS[BISHOP] +
                          2 * PIECE_WEIGHTS[ROOK] +
                          1 * PIECE_WEIGHTS[QUEEN]);


int64_t PIECE_BASE_VALUES[GAME_PHASE_NUM][PIECE_KIND_NUM] = {
    {0, 300, 900, 945, 1500, 2700, 0},
    {0, 360, 900, 900, 1500, 2700, 0}
};

int64_t PIECE_MOBILITY_BONUS[GAME_PHASE_NUM][PIECE_KIND_NUM] = {
    {0, 2, 6, 3, 3, 2, 2},
    {0, 2, 6, 4, 4, 6, 2},
};

int64_t PIECE_POSITIONAL_VALUES[GAME_PHASE_NUM][PIECE_KIND_NUM][SQUARE_NUM] = {
    { // middle game
        { },
        { // pawn
             0,  0,  0,   0,   0,  0,  0,  0,
            20, 20, 20, -10, -10, 20, 20, 20,
            20, 20, 20,  10,  10, 20, 20, 20,
            -5, -5, 10,  20,  20, 10, -5, -5,
            -5, -5, 10,  20,  20, 10, -5, -5,
            10, 10, 10,  15,  15, 10, 10, 10,
            50, 50, 50,  50,  50, 50, 50, 50,
             0,  0,  0,   0,   0,  0,  0,  0,
        },
        { // knight
            -10, -10,  0,  0,  0,  0, -10, -10,
            -10,   0,  0, 10, 10,  0,   0, -10,
              0,   0, 12, 15, 15, 12,   0,   0,
              5,  15, 18, 25, 25, 18,  15,   5,
              5,  15, 20, 27, 27, 20,  15,   5,
              5,  15, 18, 25, 25, 18,  15,   5,
              0,   0, 10, 15, 15, 10,   0,   0,
              0,   0, 10, 15, 15, 10,   0,   0,
        },
        { // bishop
             0,  0,  5,  0,  0,  5,  0,  0,
            10, 20, 10,  5,  5, 10, 20, 10,
            15, 20, 15, 10, 10, 15, 20, 15,
            15, 20, 15, 20, 20, 15, 20, 15,
            15, 20, 15, 20, 20, 15, 20, 15,
            10, 15, 15, 10, 10, 15, 15, 10,
             0,  7,  7,  5,  5,  7,  7,  0,
             0,  7,  7,  5,  5,  7,  7,  0,
        },
        { // rook
              0,  0,  0, 10, 10,  0,  0,   0,
            -10,  0,  0,  0,  0,  0,  0, -10,
            -10,  0,  0,  0,  0,  0,  0, -10,
            -10,  0,  0,  0,  0,  0,  0, -10,
            -10,  0,  0,  0,  0,  0,  0, -10,
            -10,  0,  0,  0,  0,  0,  0, -10,
            -10,  0,  0,  0,  0,  0,  0, -10,
              0,  0,  0,  0,  0,  0,  0,   0,
        },
        { // queen
            -10, -10,  0,  0,  0,  0, -10, -10,
            -10,   0,  0,  0,  0,  0,   0, -10,
              0,   0,  0,  0,  0,  0,   0,   0,
              0,   0,  0,  0,  0,  0,   0,   0,
              0,   0,  0,  0,  0,  0,   0,   0,
              0,   0,  0,  0,  0,  0,   0,   0,
            -10,   0,  0,  0,  0,  0,   0, -10,
            -10, -10,  0,  0,  0,  0, -10, -10,
        },
        { // king
             20,  25,  15, -10, -10,  15,  25,  20,
              5,   5, -13, -20, -20, -13,   5,   5,
            -20, -25, -25, -40, -40, -25, -25, -20,
            -30, -35, -35, -50, -50, -35, -35, -30,
            -30, -35, -35, -50, -50, -35, -35, -30,
            -40, -45, -45, -50, -50, -45, -45, -40,
            -40, -45, -45, -50, -50, -45, -45, -40,
            -40, -45, -45, -50, -50, -45, -45, -40,
        },
    },
    { // end game
        { },
        { // pawn
             0,  0,  0,  0,  0,  0,  0,  0,
             0,  0,  0,  0,  0,  0,  0,  0,
            10, 10, 10, 20, 20, 10, 10, 10,
            20, 20, 20, 30, 30, 20, 20, 20,
            25, 25, 25, 35, 35, 25, 25, 25,
            30, 30, 30, 40, 40, 30, 30, 30,
            40, 40, 40, 50, 50, 40, 40, 40,
             0,  0,  0,  0,  0,  0,  0,  0,
        },
        { // knight
            -10, -10,  0,  0,  0,  0, -10, -10,
            -10,   0,  0, 10, 10,  0,   0, -10,
              0,   0, 10, 15, 15, 10,   0,   0,
              5,  15, 20, 25, 25, 20,  15,   5,
              5,  15, 20, 25, 25, 20,  15,   5,
              5,  15, 20, 25, 25, 20,  15,   5,
              0,   0, 10, 15, 15, 10,   0,   0,
              0,   0, 10, 15, 15, 10,   0,   0,
        },
        { // bishop
             0,  0,  5,  0,  0,  5,  0,  0,
            10, 20, 10,  5,  5, 10, 20, 10,
            15, 15, 15, 10, 10, 20, 20, 15,
            15, 15, 15, 20, 20, 20, 20, 15,
            15, 15, 15, 20, 20, 20, 20, 15,
            10, 15, 15, 10, 10, 15, 15, 10,
             0, 10, 10,  5,  5, 10, 10,  0,
             0, 10, 10,  5,  5, 10, 10,  0,
        },
        { // rook
              0,  0,  0, 10, 10,  0,  0,   0,
            -10,  0,  0,  0,  0,  0,  0, -10,
            -10,  0,  0,  0,  0,  0,  0, -10,
            -10,  0,  0,  0,  0,  0,  0, -10,
            -10,  0,  0,  0,  0,  0,  0, -10,
            -10,  0,  0,  0,  0,  0,  0, -10,
            -10,  0,  0,  0,  0,  0,  0, -10,
              0,  0,  0,  0,  0,  0,  0,   0,
        },
        { // queen
             0,  0,  0,  0,  0,  0,  0,  0,
            10, 10, 10, 10, 10, 10, 10, 10,
            20, 20, 20, 20, 20, 20, 20, 20,
            20, 20, 20, 20, 20, 20, 20, 20,
            20, 20, 20, 20, 20, 20, 20, 20,
            20, 20, 20, 20, 20, 20, 20, 20,
            10, 10, 10, 10, 10, 10, 10, 10,
             0,  0,  0,  0,  0,  0,  0,  0,
        },
        { // king
            0,  0,  0,  0,  0,  0,  0, 0,
            0, 10, 10, 10, 10, 10, 10, 0,
            0, 10, 20, 20, 20, 20, 10, 0,
            0, 10, 20, 40, 40, 20, 10, 0,
            0, 10, 20, 40, 40, 20, 10, 0,
            0, 10, 20, 20, 20, 20, 10, 0,
            0, 10, 10, 10, 10, 10, 10, 0,
            0,  0,  0,  0,  0,  0,  0, 0,
        },
    }
};

int64_t ROOK_SEMIOPEN_FILE_BONUS[GAME_PHASE_NUM] = {10, 20};
int64_t ROOK_OPEN_FILE_BONUS[GAME_PHASE_NUM]     = {20, 40};
int64_t BISHOP_PAIR_BONUS[GAME_PHASE_NUM]        = {50, 60};
int64_t PASSED_PAWN_BONUS[GAME_PHASE_NUM]        = {0, 50};
int64_t DOUBLED_PAWN_PENALTY[GAME_PHASE_NUM]     = {-30, -60};
int64_t ISOLATED_PAWN_PENALTY[GAME_PHASE_NUM]    = {-40, -80};
int64_t CONNECTED_ROOKS_BONUS[GAME_PHASE_NUM]    = {40, 20};

Square flip(Square square)
{
    return make_square(RANK_8 - rank(square), file(square));
}

PositionScorer::PositionScorer()
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

    return ((mg * phase_weight) + (eg * (MAX_WEIGHT - phase_weight))) / MAX_WEIGHT;
}

template <Color side, GamePhase phase>
int64_t PositionScorer::score(const Position& position)
{
    int64_t value = 0LL;

    for (PieceKind piecekind : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN})
    {
        Piece piece = make_piece(side, piecekind);
        int num_pieces = position.number_of_pieces(piece);
        value += PIECE_BASE_VALUES[phase][piecekind] * num_pieces;
        value += PIECE_MOBILITY_BONUS[phase][piecekind] * move_count[side][piecekind];

        for (int i = 0; i < num_pieces; ++i)
        {
            Square square = position.piece_position(piece, i);
            square = side == WHITE ? square : flip(square);
            value += PIECE_POSITIONAL_VALUES[phase][piecekind][square];
        }
    }

    Square square = position.piece_position(make_piece(side, KING), 0);
    square = side == WHITE ? square : flip(square);
    value += PIECE_POSITIONAL_VALUES[phase][KING][square];

    constexpr Piece ROOK_PIECE = make_piece(side, ROOK);
    for (int i = 0; i < position.number_of_pieces(ROOK_PIECE); ++i)
    {
        Square square = position.piece_position(ROOK_PIECE, i);
        Bitboard file_bb = FILES_BB[file(square)];

        if (!(file_bb & position.pieces(PAWN)))
            value += ROOK_OPEN_FILE_BONUS[phase];
        if (!(file_bb & position.pieces(side, PAWN)) &&
                file_bb & position.pieces(!side, PAWN))
            value += ROOK_SEMIOPEN_FILE_BONUS[phase];
    }

    if (position.number_of_pieces(make_piece(side, ROOK)) == 2)
    {
        Square rook1 = position.piece_position(make_piece(side, ROOK), 0);
        Square rook2 = position.piece_position(make_piece(side, ROOK), 1);

        if (file(rook1) == file(rook2) || rank(rook1) == rank(rook2))
        {
            Bitboard path = LINES[rook1][rook2] & ~square_bb(rook1) & ~square_bb(rook2);
            if (!(position.pieces() & path))
                value += CONNECTED_ROOKS_BONUS[phase];
        }
    }

    if ((position.pieces(side, BISHOP) & white_squares_bb) &&
            (position.pieces(side, BISHOP) & black_squares_bb))
        value += BISHOP_PAIR_BONUS[phase];

    constexpr Piece PAWN_PIECE = make_piece(side, PAWN);

    Bitboard pawns = position.pieces(side, PAWN);

    for (File file = FILE_A; file <= FILE_H; ++file)
        if (popcount_more_than_one(pawns & FILES_BB[file]))
             value += DOUBLED_PAWN_PENALTY[phase];

    for (int i = 0; i < position.number_of_pieces(PAWN_PIECE); ++i)
    {
        Square square = position.piece_position(PAWN_PIECE, i);
        if (!(NEIGHBOUR_FILES_BB[file(square)] & pawns))
            value += ISOLATED_PAWN_PENALTY[phase];

        if (!(passed_pawn_bb(side, square) & position.pieces(!side, PAWN)))
            value += PASSED_PAWN_BONUS[phase];
    }

    return value;
}

int64_t PositionScorer::game_phase_weight(const Position& position)
{
    int64_t weight = 0;
    weight += position.number_of_pieces(W_KNIGHT) * PIECE_WEIGHTS[KNIGHT];
    weight += position.number_of_pieces(W_BISHOP) * PIECE_WEIGHTS[BISHOP];
    weight += position.number_of_pieces(W_ROOK) * PIECE_WEIGHTS[ROOK];
    weight += position.number_of_pieces(W_QUEEN) * PIECE_WEIGHTS[QUEEN];
    weight += position.number_of_pieces(B_KNIGHT) * PIECE_WEIGHTS[KNIGHT];
    weight += position.number_of_pieces(B_BISHOP) * PIECE_WEIGHTS[BISHOP];
    weight += position.number_of_pieces(B_ROOK) * PIECE_WEIGHTS[ROOK];
    weight += position.number_of_pieces(B_QUEEN) * PIECE_WEIGHTS[QUEEN];

    return std::min(weight, MAX_WEIGHT);
}

}
