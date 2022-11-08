#ifndef CHESS_ENGINE_HASHMAP_H_
#define CHESS_ENGINE_HASHMAP_H_

#include <vector>

namespace engine
{

    template <typename Key, typename Value, size_t Size>
    class HashMap
    {
        static_assert(!(Size & (Size - 1)), "Size must be a power of 2");

        public:
            using Entry = std::pair<Key, Value>;

            HashMap() { }

            Value* operator[] (const Key& key) { return &data_[key & (Size - 1)].second; }

            Entry* probe(const Key& key, bool& found)
            {
                Entry* entry = &data_[key & (Size - 1)];
                found = (entry->first == key);
                return entry;
            }

            void insert(const Key& key, const Value& value)
            {
                data_[key & (Size - 1)] = std::make_pair(key, value);
            }

            void clear()
            {
                for (size_t i = 0; i < Size; ++i)
                {
                    data_[i].first = 0ULL;
                }
            }

        private:
            std::vector<Entry> data_ = std::vector<Entry>(Size);
    };
}

#endif  // CHESS_ENGINE_HASHMAP_H_
