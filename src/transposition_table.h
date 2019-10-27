#ifndef CHESS_ENGINE_TRANSPOSITION_TABLE_H_
#define CHESS_ENGINE_TRANSPOSITION_TABLE_H_

#include "types.h"
#include "position.h"

namespace engine
{
namespace transposition
{

struct Entry
{
    Position position;
    Move best_move;
    int32_t depth;
    int64_t value;
};

void init(size_t size);

void update(HashKey key, Entry entry);

bool contains(HashKey key);

Entry get(HashKey key);

}  // namespace transposition
}  // namespace engine

#endif  // CHESS_ENGINE_TRANSPOSITION_TABLE_H_
