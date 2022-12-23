#include "endgame.h"

#include "bithacks.h"
#include "types.h"
#include "value.h"

#include <cassert>
#include <cstdlib>
#include <memory>
#include <vector>

namespace engine
{
namespace endgame
{

enum EndgameType : uint32_t
{
    kKPK,
    kKPsK,
    kKNBK,
    kKXK,
    kKQKR,
    kKRNKR,
    kKRBKR,
    kKBPsK,
};

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
// clang-format on


/**
 * Weights to have both kings close to each other.
 */
int PUSH_CLOSE[RANK_NUM] = {0, 7, 6, 5, 4, 3, 2, 1};

Square most_advanced_pawns(Bitboard pawns, Color side)
{
    return Square(side == WHITE ? msb(pawns) : lsb(pawns));
}

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

template <>
struct SandboxPCV<kKRNKR> {
    static constexpr PieceCountVector pcv[COLOR_NUM] = {
        create_pcv(0, 1, 0, 1, 0, 0, 0, 0, 1, 0),
        create_pcv(0, 0, 0, 1, 0, 0, 1, 0, 1, 0)
    };
};

template <>
struct SandboxPCV<kKRBKR> {
    static constexpr PieceCountVector pcv[COLOR_NUM] = {
        create_pcv(0, 0, 1, 1, 0, 0, 0, 0, 1, 0),
        create_pcv(0, 0, 0, 1, 0, 0, 0, 1, 1, 0)
    };
};

template <>
bool Endgame<kKPK>::applies(const Position& position) const
template <EndgameType endgameType>
class Endgame : public EndgameBase
{
  public:
    Endgame(Color strong) : EndgameBase(strong) {}

    bool applies(const Position& position) const override
    {
        return position.get_pcv() == SandboxPCV<endgameType>::pcv[strongSide];
    }

    virtual ~Endgame() = default;

  protected:
    Value strongSideScore(const Position& position) const override;
};

template <>
Value Endgame<kKPK>::strongSideScore(const Position& position) const
{
    assert(applies(position));

    Color side = position.color();
    Square strongKingSq = position.piece_position(strongKing, 0);
    Square weakKingSq = position.piece_position(weakKing, 0);
    Square strongPawn =
        position.piece_position(make_piece(strongSide, PAWN), 0);

    bitbase::normalize(strongSide, side, strongKingSq, strongPawn, weakKingSq);
    if (!bitbase::check(side, strongKingSq, strongPawn, weakKingSq))
        return VALUE_POSITIVE_DRAW + Value(rank(normalize(strongPawn, strongSide)));

    return VALUE_KNOWN_WIN + Value(rank(normalize(strongPawn, strongSide)));
}

template <>
bool Endgame<kKPsK>::applies(const Position& position) const
{
    return position.no_nonpawns(strongSide) == 0
        && position.number_of_pieces(make_piece(strongSide, PAWN)) >= 2
        && position.pieces(weakSide) == position.pieces(weakSide, KING);
}

template <>
Value Endgame<kKPsK>::strongSideScore(const Position& position) const
{
    const Bitboard pawns = position.pieces(strongSide, PAWN);
    const File pawnFile = file(Square(lsb(pawns)));
    const Square queeningSq = normalize(make_square(RANK_8, pawnFile), strongSide);
    const Square weakKingSq = position.piece_position(make_piece(weakSide, KING));

    // It's a draw if:
    // 1. All pawns are on the same rook file (A or H).
    // 2. Weak king can reach queening square.
    if ((pawns == (pawns & fileA_bb) || pawns == (pawns & fileH_bb)) &&
            distance(weakKingSq, queeningSq) <= 1)
    {
        return VALUE_POSITIVE_DRAW;
    }

    return VALUE_KNOWN_WIN
         + PIECE_VALUE[PAWN].eg * position.number_of_pieces(make_piece(strongSide, PAWN))
         + Value(rank(normalize(most_advanced_pawns(pawns, strongSide), strongSide)));
}

template <>
Value Endgame<kKNBK>::strongSideScore(const Position& position) const
{
    Square weakKingSq = position.piece_position(weakKing, 0);
    Square bishop = position.piece_position(make_piece(strongSide, BISHOP), 0);
    Color bishopColor = (static_cast<int>(rank(bishop)) + static_cast<int>(file(bishop))) & 1 ? WHITE : BLACK;

    Square kingSquare =
        bishopColor == WHITE ? flip_vertically(weakKingSq) : weakKingSq;
    Value v = Value(PUSH_TO_COLOR_CORNER_BONUS[kingSquare]);

    return Value(std::min(int64_t(VALUE_KNOWN_WIN + v), int64_t(VALUE_MATE - 1)));
}

template <>
Value Endgame<kKQKR>::strongSideScore(const Position& position) const
{
    Square strongKingSq = position.piece_position(strongKing, 0);
    Square weakKingSq = position.piece_position(weakKing, 0);

    Value v = PIECE_VALUE[QUEEN].eg - PIECE_VALUE[ROOK].eg
            + PUSH_TO_EDGE_BONUS[weakKingSq]
            + PUSH_CLOSE[distance(strongKingSq, weakKingSq)];

    return Value(std::min(int64_t(VALUE_KNOWN_WIN + v), int64_t(VALUE_MATE - 1)));
}

template <>
bool Endgame<kKXK>::applies(const Position& position) const
{
    return popcount(position.pieces(weakSide)) == 1;
}

template <>
Value Endgame<kKXK>::strongSideScore(const Position& position) const
{
    const Square strongKingSq = position.piece_position(strongKing, 0);
    const Square weakKingSq = position.piece_position(weakKing, 0);

    Value v = VALUE_DRAW;
    v += PIECE_VALUE[PAWN].eg * position.number_of_pieces(make_piece(strongSide, PAWN));
    v += PIECE_VALUE[KNIGHT].eg * position.number_of_pieces(make_piece(strongSide, KNIGHT));
    v += PIECE_VALUE[BISHOP].eg * position.number_of_pieces(make_piece(strongSide, BISHOP));
    v += PIECE_VALUE[ROOK].eg * position.number_of_pieces(make_piece(strongSide, ROOK));
    v += PIECE_VALUE[QUEEN].eg * position.number_of_pieces(make_piece(strongSide, QUEEN));
    v += PUSH_TO_EDGE_BONUS[weakKingSq] +
         PUSH_CLOSE[distance(strongKingSq, weakKingSq)];

    return Value(std::min(int64_t(v + VALUE_KNOWN_WIN), int64_t(VALUE_MATE - 1)));
}

template <>
Value Endgame<kKRNKR>::strongSideScore(const Position& position) const
{
    const Square weakKingSq = position.piece_position(weakKing, 0);
    return VALUE_POSITIVE_DRAW + PUSH_TO_EDGE_BONUS[weakKingSq];
}

template <>
Value Endgame<kKRBKR>::strongSideScore(const Position& position) const
{
    const Square weakKingSq = position.piece_position(weakKing, 0);
    return VALUE_POSITIVE_DRAW + PUSH_TO_EDGE_BONUS[weakKingSq];
}

template <>
bool Endgame<kKRBKR>::applies(const Position& position) const
{
    return position.get_pcv() == SandboxPCV<kKRBKR>::pcv[strongSide];
}

template <>
bool Endgame<kKBPsK>::applies(const Position& position) const
{
    return position.number_of_pieces(make_piece(strongSide, BISHOP)) == 1
        && position.no_nonpawns(strongSide) == 1
        && position.number_of_pieces(make_piece(strongSide, PAWN)) > 0
        && position.pieces(weakSide) == position.pieces(weakSide, KING);
}

template <>
Value Endgame<kKBPsK>::strongSideScore(const Position& position) const
{
    const Bitboard pawns = position.pieces(strongSide, PAWN);
    const File pawnFile = file(Square(lsb(pawns)));
    const Square queeningSq = normalize(make_square(RANK_8, pawnFile), strongSide);
    const Square bishopSq = position.piece_position(make_piece(strongSide, BISHOP));
    const Square weakKingSq = position.piece_position(make_piece(weakSide, KING));

    // It's a draw if:
    // 1. All pawns are on the same rook file (A or H).
    // 2. Queening square is different color than the bishop.
    // 3. Weak king can reach queening square.
    if ((pawns == (pawns & fileA_bb) || pawns == (pawns & fileH_bb)) &&
            sq_color(queeningSq) != sq_color(bishopSq) &&
            distance(weakKingSq, queeningSq) <= 1)
    {
        return VALUE_POSITIVE_DRAW;
    }

    return VALUE_KNOWN_WIN
        + PIECE_VALUE[PAWN].eg * position.number_of_pieces(make_piece(strongSide, PAWN))
        + PIECE_VALUE[BISHOP].eg
        + Value(rank(normalize(most_advanced_pawns(pawns, strongSide), strongSide)));
}
}


}  // namespace

using EndgameBasePtr = std::unique_ptr<EndgameBase>;

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
    add<kKPsK>();
    add<kKQKR>();
    add<kKNBK>();
    add<kKRNKR>();
    add<kKRBKR>();
    add<kKBPsK>();
    add<kKQKP>();

    // kKXK must be last
    add<kKXK>();
}

Value score(const Position& position)
{
    for (const EndgameBasePtr& e : endgames)
    {
        if (e->applies(position)) return e->score(position);
    }

    return VALUE_NONE;
}

}  // namespace endgame
}  // namespace engine
