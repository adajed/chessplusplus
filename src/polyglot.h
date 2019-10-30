#ifndef CHESS_ENGINE_POLYGLOT_H_
#define CHESS_ENGINE_POLYGLOT_H_

#include <unordered_map>

namespace engine
{

class PolyglotBook
{
    public:
        using WeightedMove = std::pair<Move, int>;

        PolyglotBook(std::string path);

        HashKey hash(const Position& position) const;

        bool contains(HashKey key) const;

        Move sample_move(HashKey key, const Position& position) const;

    private:

        Move decode_move(Move move, const Position& position) const;

        std::unordered_map<HashKey, std::vector<WeightedMove>> _hashmap;
};


}

#endif  // CHESS_ENGINE_POLYGLOT_H_
