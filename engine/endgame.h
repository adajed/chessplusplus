#ifndef CHESS_ENGINE_ENDGAME_H_
#define CHESS_ENGINE_ENDGAME_H_

#include "position.h"
#include "types.h"
#include "value.h"

#include <memory>
#include <unordered_map>

namespace engine
{
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
    EndgameBase(Color strong)
        : strongSide(strong), weakSide(!strong),
          strongKing(make_piece(strongSide, KING)),
          weakKing(make_piece(weakSide, KING))
    {}

    /**
     * @brief Verifies if endgame is applicable to given position.
     */
    virtual bool applies(const Position& position) const = 0;

    /**
     * @brief Scores given position.
     * Assumes that position is applicable.
     */
    Value score(const Position& position) const
    {
        Value v = strongSideScore(position);
        return position.color() == strongSide ? v : -v;
    }

    virtual ~EndgameBase() = default;

  protected:

    /**
     * @brief Scores given position from the strong side perspective.
     * Assumes that position is applicable.
     */
    virtual Value strongSideScore(const Position& position) const = 0;

    Color strongSide, weakSide;
    Piece strongKing, weakKing;
};

void init();

Value score(const Position& position);

}  // namespace endgame

}  // namespace engine

#endif  // CHESS_ENGINE_ENDGAME_H_
