#ifndef CHESS_ENGINE_SEARCH_H_
#define CHESS_ENGINE_SEARCH_H_

#include "position.h"
#include "score.h"
#include "types.h"

namespace engine
{

struct ScoredMove
{
    Move move;
    int64_t score;
};

class Search
{
    public:
        Search(const PositionScorer& scorer);

        Move select_move(const Position& position, int depth);

        void set_thinking_time(uint64_t time);

        uint64_t get_thinking_time();

        const static int64_t WIN = 1LL << 16;
        const static int64_t LOST = -WIN;
        const static int64_t DRAW = 0LL;


    private:
        int64_t search(Position& position, int depth, int64_t alpha, int64_t beta, Move* pv);

        int64_t quiescence_search(Position& position, int depth, int64_t alpha, int64_t beta);

        PositionScorer scorer;
        uint64_t thinking_time;

        Move pv_moves[MAX_DEPTH];
};

}

#endif  // CHESS_ENGINE_SEARCH_H_
