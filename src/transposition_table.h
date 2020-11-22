#ifndef CHESS_ENGINE_TRANSPOSITION_TABLE_H_
#define CHESS_ENGINE_TRANSPOSITION_TABLE_H_

#include <unordered_map>

#include "types.h"
#include "position.h"

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
    TTEntry(int64_t score, int32_t depth, Flag flag, Move move)
        : score(score), depth(depth), flag(flag), move(move)
    {
    }

    int64_t score;
    int32_t depth;
    Flag flag;
    Move move;

};

class TTable
{
    public:
        TTable();

        void update(HashKey key, TTEntry entry);

        const TTEntry* get(HashKey key) const;

        void clear();


    private:
        std::unordered_map<HashKey, TTEntry> _hashmap;
};

}  // namespace tt
}  // namespace engine

#endif  // CHESS_ENGINE_TRANSPOSITION_TABLE_H_
