#ifndef CHESS_ENGINE_SEARCH_H_
#define CHESS_ENGINE_SEARCH_H_

#include "move_picker.h"
#include "position.h"
#include "score.h"
#include "time_manager.h"
#include "transposition_table.h"
#include "types.h"

#include <chrono>

namespace engine
{
using MoveList = std::vector<Move>;
using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

constexpr Value INFINITY_SCORE = 1LL << 32;
constexpr Value DRAW_SCORE = 0LL;

std::string score2str(Value score);

constexpr Value win_in(int ply)
{
    return INFINITY_SCORE - ply;
}

constexpr Value lost_in(int ply)
{
    return -win_in(ply);
}

class Search
{
  public:
    Search(const Position& position, const Limits& limits);

    void go();

    void stop();

  private:
    void iter_search();

    Value root_search(Position& position, int depth, Value alpha, Value beta);

    template <bool allow_null_move>
    Value search(Position& position, int depth, Value alpha, Value beta);

    Value quiescence_search(Position& position, int depth, Value alpha,
                            Value beta);

    MoveList get_pv(int length);

    void print_info(Value score, int depth, int64_t nodes_searched,
                    int64_t elapsed);

    bool check_limits();

    Position _position;
    PositionScorer _scorer;
    Limits limits;

    int64_t check_limits_counter;
    bool stop_search;

    Duration _search_time;
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

}  // namespace engine

#endif  // CHESS_ENGINE_SEARCH_H_
