#include "transposition_table.h"

#include <algorithm>
#include <cassert>
#include <unordered_map>

namespace engine
{
namespace transposition
{

const size_t MEGABYTE = 1024ULL * 1024ULL;

std::vector<Entry> hashmap;

const Entry EMPTY_ENTRY = {NO_MOVE, 0, 0};

size_t position(HashKey key)
{
    return key % hashmap.size();
}

void init(size_t size)
{
    assert(size > 0);
    hashmap = std::vector<Entry>(size * MEGABYTE / sizeof(Entry), EMPTY_ENTRY);
}

void clear()
{
    std::fill(hashmap.begin(), hashmap.end(), EMPTY_ENTRY);
}

void update(HashKey key, Entry entry)
{
    hashmap[position(key)] = entry;
}

bool contains(HashKey key)
{
    return hashmap[position(key)].best_move != NO_MOVE;
}

Entry get(HashKey key)
{
    assert(contains(key));
    return hashmap[position(key)];
}

}  // namespace transposition
}  // namespace engine
