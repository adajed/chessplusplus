#include "endgame.h"

#include "bithacks.h"
#include "types.h"

#include <cassert>
#include <memory>
#include <vector>

namespace engine
{
namespace endgame
{
namespace
{
// clang-format off
/**
 * Weights to push weak king to edges and corners.
 */
int PUSH_TO_EDGE_BONUS[SQUARE_NUM] = {
    100, 90, 80, 70, 70, 80, 90, 100,
     90, 60, 50, 40, 40, 50, 60,  90,
     80, 50, 30, 20, 20, 30, 40,  80,
     70, 40, 20, 10, 10, 20, 40,  70,
     70, 40, 20, 10, 10, 20, 40,  70,
     80, 50, 30, 20, 20, 30, 40,  80,
     90, 60, 50, 40, 40, 50, 60,  90,
    100, 90, 80, 70, 70, 80, 90, 100,
};

/**
 * Weights to push weak king to corner of correct color.
 * Default is to push to black corners, for white
 * board needs to be flipped horizentaly.
 */
int PUSH_TO_COLOR_CORNER_BONUS[SQUARE_NUM] = {
    100, 90, 80, 70, 70, 60, 50,  40,
     90, 60, 50, 40, 40, 50, 60,  50,
     80, 50, 30, 20, 20, 30, 40,  60,
     70, 40, 20, 10, 10, 20, 40,  70,
     70, 40, 20, 10, 10, 20, 40,  70,
     60, 50, 30, 20, 20, 30, 40,  80,
     50, 60, 50, 40, 40, 50, 60,  90,
     40, 50, 60, 70, 70, 80, 90, 100,
};

/**
 * Weights to have both kings close to each other.
 */
int PUSH_CLOSE[RANK_NUM] = {0, 7, 6, 5, 4, 3, 2, 1};

template <EndgameType endgameType>
struct SandboxPCV
{
};

template <>
struct SandboxPCV<kKPK> {
    static constexpr PieceCountVector pcv[COLOR_NUM] = {
        create_pcv(1, 0, 0, 0, 0, 0, 0, 0, 0, 0),
        create_pcv(0, 0, 0, 0, 0, 1, 0, 0, 0, 0)
    };
};

template <>
struct SandboxPCV<kKNBK> {
    static constexpr PieceCountVector pcv[COLOR_NUM] = {
        create_pcv(0, 1, 1, 0, 0, 0, 0, 0, 0, 0),
        create_pcv(0, 0, 0, 0, 0, 0, 1, 1, 0, 0)
    };
};

template <>
struct SandboxPCV<kKQKR> {
    static constexpr PieceCountVector pcv[COLOR_NUM] = {
        create_pcv(0, 0, 0, 0, 1, 0, 0, 0, 1, 0),
        create_pcv(0, 0, 0, 1, 0, 0, 0, 0, 0, 1)
    };
};

// clang-format on
}  // namespace

template <>
bool Endgame<kKPK>::applies(const Position& position) const
{
    return position.get_pcv() == SandboxPCV<kKPK>::pcv[strongSide];
}

template <>
Value Endgame<kKPK>::score(const Position& position) const
{
    assert(applies(position));

    Color side = position.color();
    Square strongKing =
        position.piece_position(make_piece(strongSide, KING), 0);
    Square strongPawn =
        position.piece_position(make_piece(strongSide, PAWN), 0);
    Square weakKing = position.piece_position(make_piece(weakSide, KING), 0);

    bitbase::normalize(strongSide, side, strongKing, strongPawn, weakKing);
    if (!bitbase::check(side, strongKing, strongPawn, weakKing))
        return VALUE_DRAW;

    Value v = VALUE_KNOWN_WIN + Value(rank(strongPawn));
    return (position.color() == strongSide) ? v : (-v);
}

template <>
bool Endgame<kKNBK>::applies(const Position& position) const
{
    return position.get_pcv() == SandboxPCV<kKNBK>::pcv[strongSide];
}

template <>
Value Endgame<kKNBK>::score(const Position& position) const
{
    Square weakKing = position.piece_position(make_piece(weakSide, KING), 0);
    Square bishop = position.piece_position(make_piece(strongSide, BISHOP), 0);
    Color bishopColor = (static_cast<int>(rank(bishop)) + static_cast<int>(file(bishop))) & 1 ? WHITE : BLACK;

    Square kingSquare =
        bishopColor == WHITE ? flip_vertically(weakKing) : weakKing;
    Value v = Value(PUSH_TO_COLOR_CORNER_BONUS[kingSquare]);

    v = Value(std::min(int64_t(VALUE_KNOWN_WIN + v), int64_t(VALUE_MATE - 1)));
    return (position.color() == strongSide) ? v : (-v);
}

template <>
bool Endgame<kKQKR>::applies(const Position& position) const
{
    return position.get_pcv() == SandboxPCV<kKQKR>::pcv[strongSide];
}

template <>
Value Endgame<kKQKR>::score(const Position& position) const
{
    Square strongKing =
        position.piece_position(make_piece(strongSide, KING), 0);
    Square weakKing = position.piece_position(make_piece(weakSide, KING), 0);

    Value v = (900 - 500);
    v += PUSH_TO_EDGE_BONUS[weakKing] +
         PUSH_CLOSE[distance(strongKing, weakKing)];

    v = Value(std::min(int64_t(VALUE_KNOWN_WIN + v), int64_t(VALUE_MATE - 1)));
    return (position.color() == strongSide) ? v : (-v);
}

template <>
bool Endgame<kKXK>::applies(const Position& position) const
{
    return popcount(position.pieces(weakSide)) == 1;
}

template <>
Value Endgame<kKXK>::score(const Position& position) const
{
    Square strongKing =
        position.piece_position(make_piece(strongSide, KING), 0);
    Square weakKing = position.piece_position(make_piece(weakSide, KING), 0);

    Value v = VALUE_DRAW;
    v += 100 * position.number_of_pieces(make_piece(strongSide, PAWN));
    v += 300 * position.number_of_pieces(make_piece(strongSide, KNIGHT));
    v += 300 * position.number_of_pieces(make_piece(strongSide, BISHOP));
    v += 500 * position.number_of_pieces(make_piece(strongSide, ROOK));
    v += 900 * position.number_of_pieces(make_piece(strongSide, QUEEN));
    v += PUSH_TO_EDGE_BONUS[weakKing] +
         PUSH_CLOSE[distance(strongKing, weakKing)];

    v = Value(std::min(int64_t(v + VALUE_KNOWN_WIN), int64_t(VALUE_MATE - 1)));
    return (position.color() == strongSide) ? v : (-v);
}

std::vector<EndgameBasePtr> endgames;

template <EndgameType endgameType>
void add()
{
    endgames.push_back(
        std::unique_ptr<EndgameBase>(new Endgame<endgameType>(WHITE)));
    endgames.push_back(
        std::unique_ptr<EndgameBase>(new Endgame<endgameType>(BLACK)));
}

void init()
{
    add<kKPK>();
    add<kKQKR>();
    add<kKNBK>();
    add<kKXK>();
}

}  // namespace endgame
}  // namespace engine
