#ifndef CHESS_ENGINE_SEARCH_H_
#define CHESS_ENGINE_SEARCH_H_

#include "position.h"
#include "move_picker.h"
#include "score.h"
#include "types.h"

#include <chrono>

namespace engine
{

using MoveList = std::vector<Move>;

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

using Score = int64_t;

constexpr Score INFINITY_SCORE = 1LL << 32;
constexpr Score DRAW_SCORE = 0LL;


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

        Score root_search(Position& position, int depth, Score alpha, Score beta, MoveList& pv);

        template <bool allow_null_move>
        Score search(Position& position, int depth, Score alpha, Score beta, MoveList& pv);

        Score quiescence_search(Position& position, int depth, Score alpha, Score beta);

        bool check_limits();

        Position position;
        PositionScorer scorer;
        Limits limits;

        MoveList pv_list;
        int64_t check_limits_counter;
        bool stop_search;

        int64_t _search_time;
        int64_t _search_depth;
        int _current_depth;

        Move _best_move;
        TimePoint start_time;
        int64_t nodes_searched;

        OrderingInfo info;

        std::vector<std::pair<Score, Move>> _root_moves;

};

}

#endif  // CHESS_ENGINE_SEARCH_H_
