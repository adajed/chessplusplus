#include "position.h"
#include "score.h"
#include "types.h"
#include <bits/stdint-intn.h>

namespace engine
{

const int64_t PIECE_WEIGHTS[PIECE_KIND_NUM] = {0, 0, 2, 2, 4, 8, 0};

const int64_t MAX_PIECE_WEIGTHS = 2 * (  2 * PIECE_WEIGHTS[KNIGHT]
                                       + 2 * PIECE_WEIGHTS[BISHOP]
                                       + 2 * PIECE_WEIGHTS[ROOK]
                                       + 1 * PIECE_WEIGHTS[QUEEN]);

const int64_t PIECE_VALUE[GAME_PHASE_NUM][PIECE_KIND_NUM] = {
    {0, 300, 893, 950, 1500, 2708, 0},
    {0, 370, 894, 904, 1500, 2700, 0},
};

const int64_t PIECE_POSITIONAL_VALUE[GAME_PHASE_NUM][PIECE_KIND_NUM][SQUARE_NUM] = {
    {   // MIDDLE_GAME
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        },
        {   // PAWN
            0, 0, 0, 0, 0, 0, 0, 0,
            20, 20, 20, 19, 10, 15, 15, 20,
            10, 2, 10, 20, 22, 0, -5, 10,
            20, 1, 13, 20, 20, 6, -11, 20,
            4, 14, 11, 9, 11, 11, 10, 10,
            58, 41, 44, 53, 54, 50, 43, 52,
            0, 0, 0, 0, 0, 0, 0, 0,
        },
        {   // KNIGHT
            -17, -10, -1, -6, 0, 0, -10, -10,
            -8, 2, -5, 14, 10, 0, 0, -2,
            -9, 0, 10, 9, 7, 12, 0, -2,
            12, 15, 16, 34, 15, 14, 10, -3,
            3, 22, 28, 20, 23, 20, 14, 5,
            8, 27, 11, 23, 30, 18, 16, 5,
            0, 9, 5, 12, 7, 10, -3, -4,
            0, 0, 10, 15, 15, 12, 0, 0,
        },
        {   // BISHOP
            4, 10, 5, 6, 1, 11, 10, 8,
            16, 24, 12, 5, 5, 18, 17, 17,
            11, 20, 23, 0, 14, 9, 25, 15,
            10, 20, 13, 12, 20, 20, 20, 14,
            13, 27, 25, 36, 18, 19, 20, 15,
            27, 9, 13, 7, 10, 23, 19, 10,
            -14, 17, 14, 5, 7, 17, 13, -3,
            -6, 7, -10, 5, 13, 10, 7, 0,
        },
        {   // ROOK
            1, 0, 5, 10, -2, 0, 0, 14,
            -1, 0, 6, 0, 1, 0, -14, -10,
            -10, 2, 0, -1, 4, 0, 8, -20,
            -10, 8, 0, 5, -9, -3, 0, -10,
            -13, 0, 5, -7, 0, 0, 11, -15,
            -8, -10, 0, 0, 0, -1, 0, -10,
            -10, -1, 0, 3, 0, -13, 0, -10,
            0, -11, 0, -2, -9, 2, 3, -1,
        },
        {   // QUEEN
            0, -6, 2, -3, -1, -4, 11, -1,
            6, 18, 11, 5, 6, 14, 12, 10,
            24, 20, 20, 20, 20, 29, 25, 20,
            20, 20, 22, 20, 13, 20, 20, 15,
            19, 20, 30, 19, 19, 19, 13, 24,
            20, 25, 20, 5, 20, 25, 38, 30,
            2, 0, -1, 10, -2, 10, 19, 11,
            -1, -13, 8, 3, 0, -10, 0, -1,
        },
        {   // KING
            32, 25, 24, -20, -10, 6, 25, 11,
            0, 0, 12, -20, -20, -10, 13, 5,
            -16, -25, -28, -39, -36, -23, -25, -23,
            -31, -35, -39, -51, -59, -45, -35, -50,
            -19, -50, -33, -46, -50, -35, -28, -34,
            -40, -51, -45, -45, -40, -41, -35, -40,
            -40, -37, -33, -55, -59, -44, -45, -46,
            -49, -45, -45, -48, -50, -39, -56, -31,
        }
    },
    {   // END_GAME
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        },
        {   // PAWN
            0, 0, 0, 0, 0, 0, 0, 0,
            6, 0, -4, 0, 0, -10, -6, 0,
            9, 19, 2, 20, 23, 10, 10, 10,
            20, 22, 20, 46, 30, 11, 18, 20,
            26, 17, 16, 48, 35, 26, 33, 20,
            20, 30, 30, 40, 43, 30, 25, 36,
            40, 40, 29, 50, 50, 40, 44, 44,
            0, 0, 0, 0, 0, 0, 0, 0,
        },
        {   // KNIGHT
            -10, -10, 0, 6, -8, 0, -18, 3,
            -10, 0, 0, 12, 18, -9, -8, -14,
            0, 0, 10, 9, 15, 20, -13, 0,
            18, 7, 10, 23, 19, 20, 9, -4,
            2, 33, 10, 18, 23, 17, 15, 1,
            5, 15, 16, 25, 25, 31, 15, 5,
            0, 0, 10, 7, 21, 9, -8, 0,
            -12, -10, 4, 12, 28, -1, -4, 5,
        },
        {   // BISHOP
            0, -10, 3, -7, 10, 9, -3, 0,
            4, 20, -1, 5, 2, 10, 15, 10,
            8, 15, 57, 10, 2, 22, 17, 15,
            24, 8, 11, 20, 5, 15, 18, 23,
            15, 22, 9, 21, 20, 12, 15, 9,
            10, 19, 17, 10, 11, 20, 10, 2,
            -5, 15, 19, 19, 5, 10, 24, 9,
            0, 4, 10, 3, 5, 6, 10, 5,
        },
        {   // ROOK
            1, 0, 5, 10, -2, 0, 0, 14,
            -1, 0, 6, 0, 1, 0, -14, -10,
            -10, 2, 0, -1, 4, 0, 8, -20,
            -10, 8, 0, 5, -9, -3, 0, -10,
            -13, 0, 5, -7, 0, 0, 11, -15,
            -8, -10, 0, 0, 0, -1, 0, -10,
            -10, -1, 0, 3, 0, -13, 0, -10,
            0, -11, 0, -2, -9, 2, 3, -1,
        },
        {   // QUEEN
            0, -6, 2, -3, -1, -4, 11, -1,
            6, 18, 11, 5, 6, 14, 12, 10,
            24, 20, 20, 20, 20, 29, 25, 20,
            20, 20, 22, 20, 13, 20, 20, 15,
            19, 20, 30, 19, 19, 19, 13, 24,
            20, 25, 20, 5, 20, 25, 38, 30,
            2, 0, -1, 10, -2, 10, 19, 11,
            -1, -13, 8, 3, 0, -10, 0, -1,
        },
        {   // KING
            -9, 7, -16, -6, 3, 7, -6, 2,
            2, 13, 15, 16, 23, 10, 11, 3,
            1, 19, 18, 13, 20, 20, 19, -19,
            0, 6, 23, 40, 40, 3, 10, 0,
            0, 19, 31, 40, 43, 28, 8, -5,
            2, 10, 20, 20, 3, 17, 11, 0,
            -8, 10, 10, 6, 0, 11, 9, 5,
            -5, 0, 8, 0, -10, 0, 9, 0,
        }
    }
};

const int64_t MOBILITY_BONUS[GAME_PHASE_NUM][PIECE_KIND_NUM] =
{
    {0, 3, 6, 9, 2, 2, 0},
    {0, 2, 12, 4, 12, 6, 2}
};

// bonus for rook on semiopen file
const int64_t ROOK_SEMIOPEN_FILE_BONUS[GAME_PHASE_NUM] = {10, 11};

// bonus for rook on open file
const int64_t ROOK_OPEN_FILE_BONUS[GAME_PHASE_NUM] = {20, 40};

// bonus for bishop pair
const int64_t BISHOP_PAIR_BONUS[GAME_PHASE_NUM] = {46, 61};

// bonus for passed pawn
const int64_t PASSED_PAWN_BONUS[GAME_PHASE_NUM] = {5, 52};

// bonus for connecting rooks
const int64_t CONNECTED_ROOKS_BONUS[GAME_PHASE_NUM] = {40, 20};

// penalty for double pawns
const int64_t DOUBLE_PAWN_PENALTY[GAME_PHASE_NUM] = {-14, -57};

// penalty for isolated pawn
const int64_t ISOLATED_PAWN_PENALTY[GAME_PHASE_NUM] = {-40, -80};


Square flip(Square square)
{
    return make_square(RANK_8 - rank(square), file(square));
}

PositionScorer::PositionScorer()
    : _pawn_hash_table()
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

    int64_t phase_weight = game_phase_weight(position);

    if (position.side_to_move() == WHITE)
        return score_side<WHITE>(position, phase_weight)
            + score_pawns<WHITE>(position, phase_weight);
    else
        return score_side<BLACK>(position, phase_weight)
            + score_pawns<BLACK>(position, phase_weight);
}

template <Color side>
int64_t PositionScorer::score_side(const Position& position, int64_t weight)
{
    int64_t mg = score<WHITE, MIDDLE_GAME>(position) - score<BLACK, MIDDLE_GAME>(position);
    int64_t eg = score<WHITE, END_GAME>(position) - score<BLACK, END_GAME>(position);

    int64_t value = ((mg * weight) + (eg * (MAX_PIECE_WEIGTHS - weight))) / MAX_PIECE_WEIGTHS;
    return side == WHITE ? value : (-value);
}

template <Color side, GamePhase phase>
int64_t PositionScorer::score(const Position& position)
{
    int64_t value = 0LL;

    for (PieceKind piecekind : {KNIGHT, BISHOP, ROOK, QUEEN})
    {
        Piece piece = make_piece(side, piecekind);
        int num_pieces = position.number_of_pieces(piece);
        value += PIECE_VALUE[phase][piecekind] * num_pieces;
        value += MOBILITY_BONUS[phase][piecekind] * move_count[side][piecekind];

        for (int i = 0; i < num_pieces; ++i)
        {
            Square square = position.piece_position(piece, i);
            square = side == WHITE ? square : flip(square);
            value += PIECE_POSITIONAL_VALUE[phase][piecekind][square];
        }
    }

    Square square = position.piece_position(make_piece(side, KING), 0);
    square = side == WHITE ? square : flip(square);
    value += PIECE_POSITIONAL_VALUE[phase][KING][square];

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

    return value;
}

template <Color side>
int64_t PositionScorer::score_pawns(const Position& position, int64_t weight)
{
    std::pair<int64_t, int64_t> score;

    // check in hash table
    if (!_pawn_hash_table.get(position, score))
    {
        score.first = calculate_pawns<WHITE, MIDDLE_GAME>(position)
                    - calculate_pawns<BLACK, MIDDLE_GAME>(position);
        score.second = calculate_pawns<WHITE, END_GAME>(position)
                    - calculate_pawns<BLACK, END_GAME>(position);
        _pawn_hash_table.update(position, score);
    }

    int64_t value = (score.first * weight + score.second * (MAX_PIECE_WEIGTHS - weight)) / MAX_PIECE_WEIGTHS;
    return side == WHITE ? value : (-value);
}

template <Color side, GamePhase phase>
int64_t PositionScorer::calculate_pawns(const Position& position)
{
    constexpr Piece pawn = make_piece(side, PAWN);

    int64_t value = 0LL;

    int num_pieces = position.number_of_pieces(pawn);
    value += PIECE_VALUE[phase][PAWN] * num_pieces;
    value += MOBILITY_BONUS[phase][PAWN] * move_count[side][PAWN];

    for (int i = 0; i < num_pieces; ++i)
    {
        Square square = position.piece_position(pawn, i);
        square = side == WHITE ? square : flip(square);
        value += PIECE_POSITIONAL_VALUE[phase][PAWN][square];
    }

    Bitboard pawns = position.pieces(side, PAWN);

    for (File file = FILE_A; file <= FILE_H; ++file)
        if (popcount_more_than_one(pawns & FILES_BB[file]))
             value += DOUBLE_PAWN_PENALTY[phase];

    for (int i = 0; i < position.number_of_pieces(pawn); ++i)
    {
        Square square = position.piece_position(pawn, i);
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

    return std::min(weight, MAX_PIECE_WEIGTHS);
}

}
