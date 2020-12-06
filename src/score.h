#ifndef CHESS_ENGINE_SCORE_H_
#define CHESS_ENGINE_SCORE_H_

#include "movegen.h"
#include "pawn_hash_table.h"
#include "types.h"

namespace engine
{

class PositionScorer
{
    public:
        using Score = int64_t;

        PositionScorer();

        Score score(const Position& position);

        void clear();

    private:

        template <Color side>
        Score score_side(const Position& position, int64_t weight);

        template <Color side, GamePhase phase>
        Score score(const Position& position);

        template <Color side>
        Score score_pawns(const Position& position, int64_t weight);

        template <Color side, GamePhase phase>
        Score calculate_pawns(const Position& position);

        int64_t game_phase_weight(const Position& position);

        Move move_list[MAX_MOVES];
        int move_count[COLOR_NUM][PIECE_KIND_NUM];

        PawnHashTable _pawn_hash_table;
};

}

#endif  // CHESS_ENGINE_SCORE_H_
