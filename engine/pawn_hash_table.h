#ifndef CHESS_ENGINE_PAWN_HASH_TABLE_H_
#define CHESS_ENGINE_PAWN_HASH_TABLE_H_

#include "position.h"

#include <bits/stdint-intn.h>
#include <unordered_map>

namespace engine
{
class PawnHashTable
{
  public:
    using Value = std::pair<int64_t, int64_t>;

    PawnHashTable();

    bool get(const Position& position, Value& result) const;

    void update(const Position& position, Value score);

    void clear();

  private:
    std::unordered_map<uint64_t, Value> _hashmap;
};

}  // namespace engine

#endif  // CHESS_ENGINE_PAWN_HASH_TABLE_H_
