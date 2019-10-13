#include "position.h"
#include "score.h"
#include "types.h"

namespace engine
{

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

    for (PieceKind p = PAWN; p != KING; ++p)
    {
        Bitboard bb = pieces_bb(position, side, p);
        value += weights.get_piece_value(phase, p) * popcount(bb);
        while (bb)
        {
            Square sq = Square(pop_lsb(&bb));
            sq = side == WHITE ? sq : make_square(RANK_8 - rank(sq), file(sq));
            value += weights.get_piece_position_value(phase, p, sq);
        }
    }

    Square king_sq = position.piece_position[make_piece(side, KING)][0];
    value += weights.get_piece_position_value(phase, KING, king_sq);

    Move* begin = const_cast<Move*>(move_list);
    Move* end = generate_moves(position, begin);
    value += (end - begin) * weights.get_mobility_bonus();

    Bitboard bb = attacked_squares(position);
    value += popcount(bb) * weights.get_attacking_bonus();

    return value;
}

}
