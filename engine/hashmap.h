#ifndef CHESS_ENGINE_HASHMAP_H_
#define CHESS_ENGINE_HASHMAP_H_

#include <cstdint>
#include <vector>

namespace engine
{

    template <typename Key, typename Value, std::size_t Size>
    class HashMap
    {
        static_assert(!(Size & (Size - 1)), "Size must be a power of 2");
        static_assert(Size >= 1000, "Size must be >= 1000");

        public:
            struct Entry
            {
                Key key;
                uint32_t epoch;
                Value value;
            };

            HashMap() { }

            Value* operator[] (const Key& key) { return &data_[key & (Size - 1)].value; }

            Entry* probe(const Key& key, bool& found)
            {
                Entry* entry = &data_[key & (Size - 1)];
                found = (entry->key == key);
                return entry;
            }

            void insert(const Key& key, const Value& value)
            {
                data_[key & (Size - 1)] = Entry{key, epoch_, value};
            }

            void clear()
            {
                for (std::size_t i = 0; i < Size; ++i)
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
            std::vector<Entry> data_ = std::vector<Entry>(Size);
            uint32_t epoch_ = 0;
    };
}

#endif  // CHESS_ENGINE_HASHMAP_H_
