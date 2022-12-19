#ifndef CHESS_ENGINE_ENDGAME_H_
#define CHESS_ENGINE_ENDGAME_H_

#include "position.h"
#include "types.h"
#include "value.h"

#include <memory>
#include <unordered_map>

namespace engine
{
enum EndgameType : uint32_t
{
    kKPK = 0,
    kKNBK = 1,
    kKXK = 2,
    kKQKR = 3
};

namespace bitbase
{
void init();

bool check(Color side, Square wKing, Square wPawn, Square bKing);

void normalize(Color strongSide, Color& side, Square& strongKing,
               Square& strongPawn, Square& weakKing);

}  // namespace bitbase

namespace endgame
{
class EndgameBase
{
  public:
    EndgameBase(Color strong) : strongSide(strong), weakSide(!strong) {}

    virtual bool applies(const Position& position) const = 0;

    virtual Value score(const Position& position) const = 0;

    virtual ~EndgameBase() = default;

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

    virtual ~Endgame() = default;
};

using EndgameBasePtr = std::unique_ptr<EndgameBase>;

extern std::vector<EndgameBasePtr> endgames;

void init();

inline Value score_endgame(const Position& position)
{
    for (const EndgameBasePtr& e : endgames)
    {
        if (e->applies(position)) return e->score(position);
    }

    return VALUE_NONE;
}

}  // namespace endgame

}  // namespace engine

#endif  // CHESS_ENGINE_ENDGAME_H_
