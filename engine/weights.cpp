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
        for (GamePhase phase : {MIDDLE_GAME, END_GAME})
        {
            for (PieceKind piece = PAWN; piece != KING; ++piece)
                file >> piece_values[phase][piece];


            for (PieceKind piece = PAWN; piece <= KING; ++piece)
                for (Square sq = SQ_A1; sq != NO_SQUARE; ++sq)
                    file >> piece_position_values[phase][piece][sq];
        }

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

int32_t Weights::get_piece_value(GamePhase phase, PieceKind piecekind) const
{
    assert(piecekind != NO_PIECE_KIND && piecekind != KING);
    return piece_values[phase][piecekind];
}

int32_t Weights::get_piece_position_value(
        GamePhase phase, PieceKind piecekind, Square square) const
{
    assert(piecekind != NO_PIECE_KIND);
    assert(square != NO_SQUARE);
    return piece_position_values[phase][piecekind][square];
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
