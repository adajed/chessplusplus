#ifndef CHESS_ENGINE_TRANSPOSITION_TABLE_H_
#define CHESS_ENGINE_TRANSPOSITION_TABLE_H_

#include "position.h"
#include "types.h"

#include "hashmap.h"

namespace engine
{
namespace tt
{
enum class Flag
{
    kEXACT = 0,
    kLOWER_BOUND = 1,
    kUPPER_BOUND = 2
};

struct TTEntry
{
    TTEntry() {}
    TTEntry(int64_t score, int32_t depth, Flag flag, Move move)
        : score(score), depth(depth), flag(flag), move(move)
    {
    }

    int64_t score;
    int32_t depth;
    Flag flag;
    Move move;
};

using TTable = HashMap<uint64_t, TTEntry, 4 * 1024 * 1024>;

}  // namespace tt
}  // namespace engine

#endif  // CHESS_ENGINE_TRANSPOSITION_TABLE_H_
