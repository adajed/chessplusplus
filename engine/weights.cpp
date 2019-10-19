#include "types.h"
#include "weights.h"

#include <fstream>

namespace engine
{

Weights load(const std::string& path)
{
    Weights weights;

    std::ifstream file(path);

    if (!file.is_open())
        throw std::runtime_error(
                "Could not open file " + path);

    for (GamePhase phase : {MIDDLE_GAME, END_GAME})
    {
        for (PieceKind piece = PAWN; piece != KING; ++piece)
        {
            file >> weights.piece_values[phase][piece];
            for (Square square = SQ_A1; square <= SQ_H8; ++square)
                file >> weights.piece_positional_values[phase][piece][square];
        }

        for (Square square = SQ_A1; square <= SQ_H8; ++square)
            file >> weights.piece_positional_values[phase][KING][square];

        for (PieceKind piece = PAWN; piece <= KING; ++piece)
            file >> weights.mobility_bonus[phase][piece];

        file >> weights.rook_semiopen_file_bonus[phase];
        file >> weights.rook_open_file_bonus[phase];
        file >> weights.bishop_pair_bonus[phase];
        file >> weights.passed_pawn_bonus[phase];
        file >> weights.doubled_pawn_penalty[phase];
        file >> weights.isolated_pawn_penalty[phase];
    }

    file.close();

    return weights;
}

}
