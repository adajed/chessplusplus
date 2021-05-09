#ifndef CHESS_ENGINE_POLYGLOT_H_
#define CHESS_ENGINE_POLYGLOT_H_

#include <map>
#include <random>

#include "position.h"
#include "types.h"

namespace engine
{

class PolyglotBook
{
    public:
        using WeightedMove = std::pair<Move, int>;

        PolyglotBook() = default;

        PolyglotBook(std::string path);

        static uint64_t hash(const Position& position);

        bool contains(uint64_t key) const;

        Move sample_move(uint64_t key, const Position& position) const;

    private:

        Move decode_move(Move move, const Position& position) const;

        std::map<uint64_t, std::vector<WeightedMove>> _hashmap;
        mutable std::mt19937 _gen;
        mutable std::uniform_int_distribution<std::mt19937::result_type> _dist;

};


}

#endif  // CHESS_ENGINE_POLYGLOT_H_
