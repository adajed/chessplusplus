#ifndef CHESS_ENGINE_SEARCH_H_
#define CHESS_ENGINE_SEARCH_H_

#include "position.h"
#include "score.h"
#include "types.h"

namespace engine
{

struct Limits
{
    Limits()
        : searchmovesnum(0)
        , ponder(false)
        , movestogo(0)
        , depth(0)
        , nodes(0)
        , movetime(0)
        , infinite(false)
    {}

    Move searchmoves[MAX_MOVES];
    int searchmovesnum;
    bool ponder;
    int timeleft[COLOR_NUM];
    int timeinc[COLOR_NUM];
    int movestogo;
    int depth;
    int nodes;
    int mate;
    int movetime;
    bool infinite;
};

class Search
{
    public:
        Search(const Position& position, const PositionScorer& scorer, const Limits& limits);

        void go();

        void stop();

        const static int64_t WIN = 1LL << 16;
        const static int64_t LOST = -WIN;
        const static int64_t DRAW = 0LL;


    private:
        int64_t search(Position& position, int depth, int64_t alpha, int64_t beta, Move* pv);

        int64_t quiescence_search(Position& position, int depth, int64_t alpha, int64_t beta);

        Position position;
        PositionScorer scorer;

        Move pv_moves[MAX_DEPTH];

        Limits limits;

        int64_t thinking_time;
        int64_t nodes_searched;
};

}

#endif  // CHESS_ENGINE_SEARCH_H_
