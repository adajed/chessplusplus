#ifndef CHESS_ENGINE_MOVE_PICKER_H_
#define CHESS_ENGINE_MOVE_PICKER_H_

#include "position.h"
#include "transposition_table.h"
#include "types.h"

#include <algorithm>
#include <vector>

namespace engine
{
struct OrderingInfo
{
  public:
    OrderingInfo(tt::TTable& ttable) : ttable(ttable)
    {
        for (int i = 0; i < 50; ++i) killers[i][0] = killers[i][1] = NO_MOVE;
        for (int c = 0; c < 2; ++c)
            for (int from = 0; from < static_cast<int>(SQUARE_NUM); ++from)
                for (int to = 0; to < static_cast<int>(SQUARE_NUM); ++to)
                    history[c][from][to] = 0;
        ply = 0;
    }

    void update_killers(int p, Move move)
    {
        killers[p][1] = killers[p][0];
        killers[p][0] = move;
    }

    void update_history(Color c, Square from, Square to, int depth)
    {
        history[c][from][to] += depth * depth;
    }

    Move killers[50][2];
    int history[COLOR_NUM][SQUARE_NUM][SQUARE_NUM];
    int ply;

    tt::TTable& ttable;
};

class MovePicker
{
  public:
    MovePicker(const Position& position, Move* begin, Move* end,
               OrderingInfo& info, bool use_info);

    bool has_next();

    Move get_next();

  private:
    void score_moves(const Position& position, OrderingInfo& info,
                     bool use_info);

    std::vector<std::pair<uint32_t, Move>> _moves;
    size_t _pos;
};

}  // namespace engine

#endif  // CHESS_ENGINE_MOVE_PICKER_H_
