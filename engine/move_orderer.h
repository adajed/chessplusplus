#ifndef CHESS_ENGINE_MOVE_ORDERER_H_
#define CHESS_ENGINE_MOVE_ORDERER_H_

#include "position.h"
#include "transposition_table.h"
#include "types.h"

#include <algorithm>
#include <vector>

namespace engine
{
class MoveOrderer
{
  public:
    MoveOrderer(tt::TTable& ttable, HistoryScore& historyScore);

    void order_moves(const Position& position, Move* begin, Move* end,
                     Info* info);

  private:
    int _scores[MAX_MOVES];
    size_t _pos;

    tt::TTable& _ttable;
    HistoryScore& _history_score;
};

}  // namespace engine

#endif  // CHESS_ENGINE_MOVE_ORDERER_H_
