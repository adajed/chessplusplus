#ifndef CHESS_ENGINE_SEARCH_H_
#define CHESS_ENGINE_SEARCH_H_

#include "move_orderer.h"
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

std::string score2str(Value score);

class Search
{
  public:
    Search(const Position& position, const Limits& limits);

    void go();

    void stop();

  private:
    void iter_search();

    Value search(Position& position, int depth, Value alpha, Value beta,
                 Info* info);

    Value quiescence_search(Position& position, int depth, Value alpha,
                            Value beta, Info* info);

    void print_info(Value score, int depth, int64_t nodes_searched,
                    int64_t elapsed, Info* info);

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

    tt::TTable _ttable;
    StackInfo _stack_info;
    HistoryScore _history_score;
    MoveOrderer _move_orderer;
};

}  // namespace engine

#endif  // CHESS_ENGINE_SEARCH_H_
