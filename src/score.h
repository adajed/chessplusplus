#ifndef CHESS_ENGINE_SCORE_H_
#define CHESS_ENGINE_SCORE_H_

#include "movegen.h"
#include "weights.h"

namespace engine
{

class PositionScorer
{
    public:
        explicit PositionScorer();

        int64_t score(const Position& position) const;

    private:

        template <Color side>
        int64_t score_side(const Position& position) const;

        template <Color side, GamePhase phase>
        int64_t score(const Position& position) const;

        int64_t game_phase_weight(const Position& position) const;

        Move move_list[MAX_MOVES];
};

}

#endif  // CHESS_ENGINE_SCORE_H_
