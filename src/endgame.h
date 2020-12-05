#ifndef CHESS_ENGINE_ENDGAME_H_
#define CHESS_ENGINE_ENDGAME_H_

#include "position.h"
#include "types.h"

namespace engine
{

enum EndgameType : uint32_t
{
    kKPK = 0,
    kKRK = 1,
    kKNBK = 2,
    kKQK = 3
};

namespace bitbase
{

void init();

bool check(Color side, Square wKing, Square wPawn, Square bKing);

void normalize(Color strongSide, Color& side, Square& strongKing, Square& strongPawn, Square& weakKing);

}

}  // namespace engine

#endif  // CHESS_ENGINE_ENDGAME_H_
