#include "transposition_table.h"

#include <cassert>
#include <unordered_map>

namespace engine
{
namespace transposition
{

using HashMap = std::unordered_map<HashKey, Entry>;

HashMap hashmap;

const size_t MEGABYTE = 1ULL << 20;

void init(size_t size)
{
    hashmap = HashMap(size * MEGABYTE / sizeof(Entry));
}

void clear()
{
    size_t size = hashmap.size();
    hashmap = HashMap(size);
}

void update(HashKey key, Entry entry)
{
    hashmap.insert_or_assign(key, entry);
}

bool contains(HashKey key)
{
    return hashmap.find(key) != hashmap.end();
}

Entry get(HashKey key)
{
    assert(contains(key));
    return hashmap[key];
}

}  // namespace transposition
}  // namespace engine
