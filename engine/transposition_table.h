#ifndef CHESS_ENGINE_TRANSPOSITION_TABLE_H_
#define CHESS_ENGINE_TRANSPOSITION_TABLE_H_

#include "engine.h"

namespace engine
{
namespace transposition
{

struct Entry
{
    int32_t depth;
    Move best_move;
    int64_t value;
};

void update(HashKey key, Entry entry);

bool contains(HashKey key);

Entry get(HashKey key);

}  // namespace transposition
}  // namespace engine

#endif  // CHESS_ENGINE_TRANSPOSITION_TABLE_H_
