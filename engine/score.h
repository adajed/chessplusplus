#ifndef CHESS_ENGINE_SCORE_H_
#define CHESS_ENGINE_SCORE_H_

#include "engine.h"
#include "movegen.h"
#include "weights.h"

namespace engine
{

class PositionScorer
{
    public:
        PositionScorer(const Weights& weights);

        int64_t score(const Position& position) const;

    private:

        template <Color side>
        int64_t score(const Position& position) const;

        Weights weights;
        Move move_list[MAX_MOVES];
};

}

#endif  // CHESS_ENGINE_SCORE_H_
