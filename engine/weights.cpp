#include "types.h"
#include "weights.h"

#include <fstream>

namespace engine
{

Weights::Weights(std::string path)
{
    std::ifstream file(path);

    if (file.is_open())
    {
        for (PieceKind p = PAWN; p != KING; ++p)
            file >> piece_values[p];


        for (PieceKind p = PAWN; p <= KING; ++p)
            for (Square sq = SQ_A1; sq != NO_SQUARE; ++sq)
                file >> piece_position_values[p][sq];

        file >> mobility_bonus;
        file >> attacking_bonus;

        file.close();
    }
    else
    {
        throw std::runtime_error(
                "Could not open file " + path);
    }
}

int32_t Weights::get_piece_value(PieceKind piecekind) const
{
    assert(piecekind != NO_PIECE_KIND && piecekind != KING);
    return piece_values[piecekind];
}

int32_t Weights::get_piece_position_value(
        PieceKind piecekind, Square square) const
{
    assert(piecekind != NO_PIECE_KIND);
    assert(square != NO_SQUARE);
    return piece_position_values[piecekind][square];
}

int32_t Weights::get_mobility_bonus() const
{
    return mobility_bonus;
}

int32_t Weights::get_attacking_bonus() const
{
    return attacking_bonus;
}


}
