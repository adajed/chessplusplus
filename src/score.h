#ifndef CHESS_ENGINE_SCORE_H_
#define CHESS_ENGINE_SCORE_H_

#include "movegen.h"

namespace engine
{

class PositionScorer
{
    public:
        PositionScorer();

        int64_t score(const Position& position);

    private:

        template <Color side>
        int64_t score_side(const Position& position);

        template <Color side, GamePhase phase>
        int64_t score(const Position& position);

        int64_t game_phase_weight(const Position& position);

        Move move_list[MAX_MOVES];
        int move_count[COLOR_NUM][PIECE_KIND_NUM];
};

}

#endif  // CHESS_ENGINE_SCORE_H_
