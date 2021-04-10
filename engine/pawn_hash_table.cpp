#include "pawn_hash_table.h"
#include <bits/stdint-intn.h>

namespace engine
{

PawnHashTable::PawnHashTable()
    : _hashmap()
{
}

bool PawnHashTable::get(const Position& position, Value& score) const
{
    uint64_t key = position.pawn_hash();
    auto result = _hashmap.find(key);
    if (result != _hashmap.end())
    {
        score =_hashmap.at(key);
        return true;
    }
    return false;
}

void PawnHashTable::update(const Position& position, Value score)
{
    uint64_t key = position.pawn_hash();
    auto result = _hashmap.insert(std::make_pair(key, score));

    if (!result.second)
        result.first->second = score;
}

void PawnHashTable::clear()
{
    _hashmap.clear();
}

}  // namespace engine
