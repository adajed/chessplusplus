#include "endgame.h"
#include "types.h"
#include <cassert>
#include <memory>
#include <vector>

namespace engine
{
namespace endgame
{

template<>
bool Endgame<kKPK>::applies(const Position& position) const
{
    if (position.number_of_pieces(make_piece(strongSide, PAWN)) != 1
            || position.number_of_pieces(make_piece(weakSide, PAWN)) != 0
            || position.pieces(KNIGHT)
            || position.pieces(BISHOP)
            || position.pieces(ROOK)
            || position.pieces(QUEEN))
        return false;

    return true;
}

template<>
Value Endgame<kKPK>::score(const Position& position) const
{
    assert(applies(position));

    Color side = position.side_to_move();
    Square strongKing = position.piece_position(make_piece(strongSide, KING), 0);
    Square strongPawn = position.piece_position(make_piece(strongSide, PAWN), 0);
    Square weakKing   = position.piece_position(make_piece(weakSide, KING), 0);

    bitbase::normalize(strongSide, side, strongKing, strongPawn, weakKing);
    if (!bitbase::check(side, strongKing, strongPawn, weakKing))
        return VALUE_DRAW;

    Value v = VALUE_KNOWN_WIN + Value(rank(strongPawn));
    return (position.side_to_move() == strongSide) ? v : (-v);
}

std::vector<EndgamePair> endgames;
EndgamePair default_endgame = std::make_pair(std::make_unique<Endgame<kKPK>>(WHITE),
                                             std::make_unique<Endgame<kKPK>>(BLACK));

template <EndgameType endgameType>
void add()
{
    endgames.push_back(std::make_pair(std::unique_ptr<EndgameBase>(new Endgame<endgameType>(WHITE)),
                                      std::unique_ptr<EndgameBase>(new Endgame<endgameType>(BLACK))));
}

void init()
{
    add<kKPK>();
}

}  // namespace endgame
}  // namespace engine
