#ifndef CHESS_ENGINE_HASHMAP_H_
#define CHESS_ENGINE_HASHMAP_H_

#include "utils.h"
#include <cstdint>
#include <iostream>
#include <ostream>
#include <vector>

namespace engine
{

constexpr uint64_t DEFAULT_TT_HASHMAP_SIZE_MB = 128;

template <typename Key, typename Value>
class HashMap
{
  public:
    struct Entry
    {
        Key key;
        uint32_t epoch;
        Value value;
    };

    static_assert(sizeof(Entry) == 32);

    HashMap(uint64_t size = DEFAULT_TT_HASHMAP_SIZE_MB * 1024 * 1024)
        : size_(size / sizeof(Entry)), data_(size_)
    {
        ASSERT(size_ >= 1000);
    }

    Value* operator[](const Key& key) { return &data_[mapKey2Pos(key)].value; }

    Entry* probe(const Key& key, bool& found)
    {
        Entry* entry = &data_[mapKey2Pos(key)];
        found = (entry->key == key);
        return entry;
    }

    void insert(const Key& key, const Value& value)
    {
        data_[mapKey2Pos(key)] = Entry{key, epoch_, value};
    }

    void clear()
    {
        for (std::size_t i = 0; i < size_; ++i)
        {
            data_[i].key = 0ULL;
        }
    }

    int32_t hashfull() const
    {
        int32_t count = 0;
        for (std::size_t i = 0; i < 1000; ++i)
        {
            count += static_cast<int32_t>(data_[i].epoch == epoch_);
        }
        return count;
    }

    void updateEpoch(uint32_t amount) { epoch_ += amount; }

    bool isCurrentEpoch(uint32_t epoch) const { return epoch == epoch_; }

  private:
    std::size_t mapKey2Pos(const Key& key) const { return key % size_; }

    uint64_t nextPowerOf2(uint64_t x) const
    {
        uint64_t power = 1ULL;
        while (power < x)
            power *= 2;
        return power;
    }

    std::size_t size_;
    std::vector<Entry> data_;
    uint32_t epoch_ = 1;
};
}  // namespace engine

#endif  // CHESS_ENGINE_HASHMAP_H_
