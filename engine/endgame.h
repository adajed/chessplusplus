#ifndef CHESS_ENGINE_ENDGAME_H_
#define CHESS_ENGINE_ENDGAME_H_

#include "position.h"
#include "types.h"
#include <memory>
#include <unordered_map>

namespace engine
{

enum EndgameType : uint32_t
{
    kKPK = 0,
    kKRK = 1,
    kKNBK = 2,
    kKQK = 3,
    kKXK = 4
};

namespace bitbase
{

void init();

bool check(Color side, Square wKing, Square wPawn, Square bKing);

void normalize(Color strongSide, Color& side, Square& strongKing, Square& strongPawn, Square& weakKing);

}  // namespace bitbase

namespace endgame
{

class EndgameBase
{
public:
    EndgameBase(Color strong) : strongSide(strong), weakSide(!strong) {}

    virtual bool applies(const Position& position) const = 0;

    virtual Value score(const Position& position) const = 0;

protected:
    Color strongSide, weakSide;
};

template <EndgameType endgameType>
class Endgame : public EndgameBase
{
public:
    Endgame(Color strong) : EndgameBase(strong) {}

    virtual bool applies(const Position& position) const;

    virtual Value score(const Position& position) const;
};

using EndgameBasePtr = std::unique_ptr<EndgameBase>;
using EndgamePair = std::pair<EndgameBasePtr, EndgameBasePtr>;

extern std::vector<EndgamePair> endgames;

extern EndgamePair default_endgame;

void init();

template <Color strongSide>
Value score_endgame(const Position& position)
{
    for (const EndgamePair& p : endgames)
    {
        const EndgameBasePtr& e = strongSide == WHITE ? p.first : p.second;

        if (e->applies(position))
            return e->score(position);
    }

    const EndgameBasePtr& e = strongSide == WHITE ? default_endgame.first : default_endgame.second;
    return e->score(position);
}

}  // namespace endgame

}  // namespace engine

#endif  // CHESS_ENGINE_ENDGAME_H_
