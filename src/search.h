#ifndef CHESS_ENGINE_SEARCH_H_
#define CHESS_ENGINE_SEARCH_H_

#include "position.h"
#include "score.h"
#include "types.h"

#include <chrono>

namespace engine
{

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;


using Score = int64_t;

constexpr Score INFINITY_SCORE = 1LL << 32;
constexpr Score DRAW_SCORE = 0LL;

Score mate_in(int ply);
bool is_score_mate(Score score);

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
    int64_t nodes;
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

    private:
        Score search(Position& position, int depth, Score alpha, Score beta, Move* pv);

        Score quiescence_search(Position& position, int depth, Score alpha, Score beta);

        bool check_limits();


        Position position;
        PositionScorer scorer;
        Limits limits;

        Move pv_moves[MAX_DEPTH];
        int64_t check_limits_counter;
        bool stop_search;

        TimePoint start_time;
        int64_t thinking_time;
        int64_t nodes_searched;
};

}

#endif  // CHESS_ENGINE_SEARCH_H_
