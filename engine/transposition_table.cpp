#include "transposition_table.h"

#include <cassert>
#include <unordered_map>

namespace engine
{
namespace transposition
{

const uint64_t HASH_MAP_SIZE = 100000000;

using HashMap = std::unordered_map<HashKey, Entry>;

HashMap hashmap(HASH_MAP_SIZE);

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
