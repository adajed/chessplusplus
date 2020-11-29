#include "transposition_table.h"

#include <cassert>
#include <unordered_map>

#include <iostream>

namespace engine
{
namespace tt
{

TTable::TTable() : _hashmap()
{
}

void TTable::update(uint64_t key, TTEntry entry)
{
   auto result = _hashmap.insert(std::make_pair(key, entry));

   if (!result.second)
   {
        result.first->second = entry;
   }
}

const TTEntry* TTable::get(uint64_t key) const
{
    auto result = _hashmap.find(key);
    if (result != _hashmap.end())
        return &_hashmap.at(key);
    return nullptr;
}

void TTable::clear()
{
    _hashmap.clear();
}

}  // namespace tt
}  // namespace engine
