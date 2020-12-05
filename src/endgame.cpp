#include "endgame.h"
#include "types.h"
#include <cassert>
#include <memory>
#include <vector>

namespace engine {

template<>
bool Endgame<kKPK>::applies(const Position& position) const
{
    if (position.number_of_pieces(make_piece(strongSide, PAWN)) != 1)
        return false;
    if (position.number_of_pieces(make_piece(weakSide, PAWN)) != 0)
        return false;
    if (!position.pieces(KNIGHT))
        return false;
    if (!position.pieces(BISHOP))
        return false;
    if (!position.pieces(ROOK))
        return false;
    if (!position.pieces(QUEEN))
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

    return VALUE_KNOWN_WIN + Value(rank(strongPawn));
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

} // namespace engine
