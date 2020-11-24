#ifndef CHESS_ENGINE_SEARCH_H_
#define CHESS_ENGINE_SEARCH_H_

#include "position.h"
#include "move_picker.h"
#include "score.h"
#include "transposition_table.h"
#include "types.h"

#include <chrono>

namespace engine
{

using MoveList = std::vector<Move>;

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

using Score = int64_t;

constexpr Score INFINITY_SCORE = 1LL << 32;
constexpr Score DRAW_SCORE = 0LL;

std::string score2str(Score score);

constexpr Score win_in(int ply)
{
    return INFINITY_SCORE - ply;
}

constexpr Score lost_in(int ply)
{
    return -win_in(ply);
}


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

        void iter_search();

        Score root_search(Position& position, int depth, Score alpha, Score beta);

        template <bool allow_null_move>
        Score search(Position& position, int depth, Score alpha, Score beta);

        Score quiescence_search(Position& position, int depth, Score alpha, Score beta);

        MoveList get_pv(int length);

        void print_info(Score score, int depth, int64_t nodes_searched, int64_t elapsed);

        bool check_limits();

        Position _position;
        PositionScorer _scorer;
        Limits limits;

        int64_t check_limits_counter;
        bool stop_search;

        int64_t _search_time;
        int64_t _search_depth;
        int64_t _max_nodes_searched;
        int _current_depth;

        Move _best_move;
        TimePoint _start_time;
        int64_t _nodes_searched;

        std::vector<Move> _root_moves;

        int _ply_counter;

        tt::TTable _ttable;
        OrderingInfo _info;


};

}

#endif  // CHESS_ENGINE_SEARCH_H_
