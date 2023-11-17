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
        : score_(score), depth_flag_score_(depth << 19 | static_cast<uint32_t>(flag) << 17 | move)
    {
    }

    int64_t score() const { return score_; }
    int32_t depth() const { return depth_flag_score_ >> 19; }
    Flag flag() const { return static_cast<Flag>((depth_flag_score_ >> 17) & 0x3); }
    Move move() const { return depth_flag_score_ & 0x1FFFF; }

private:
    int64_t score_;
    uint32_t depth_flag_score_;
    /* Flag flag_; */
    /* Move move_; */
};

using TTable = HashMap<uint64_t, TTEntry>;

}  // namespace tt
}  // namespace engine

#endif  // CHESS_ENGINE_TRANSPOSITION_TABLE_H_
