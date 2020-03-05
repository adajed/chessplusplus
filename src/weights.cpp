#include "types.h"
#include "weights.h"

#include <fstream>

namespace engine
{

Weights::Weights(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);

    auto read = [&file]() {
        int16_t temp;
        file.read(reinterpret_cast<char*>(&temp), sizeof(int16_t));
        return static_cast<int64_t>(temp);
    };

    if (!file.is_open())
        throw std::runtime_error(
                "Could not open file " + path);

    for (PieceKind piece = KNIGHT; piece != KING; ++piece)
        piece_weights[piece] = read();

    max_weight = 2 * (  2 * piece_weights[KNIGHT]
                      + 2 * piece_weights[BISHOP]
                      + 2 * piece_weights[ROOK]
                      + 1 * piece_weights[QUEEN]);

    for (GamePhase phase : {MIDDLE_GAME, END_GAME})
    {
        for (PieceKind piece = PAWN; piece != KING; ++piece)
        {
            piece_values[phase][piece] = read();
            for (Square square = SQ_A1; square <= SQ_H8; ++square)
                piece_positional_values[phase][piece][square] = read();
        }

        for (Square square = SQ_A1; square <= SQ_H8; ++square)
            piece_positional_values[phase][KING][square] = read();

        for (PieceKind piece = PAWN; piece <= KING; ++piece)
            mobility_bonus[phase][piece] = read();

        rook_semiopen_file_bonus[phase] = read();
        rook_open_file_bonus[phase] = read();
        bishop_pair_bonus[phase] = read();
        passed_pawn_bonus[phase] = read();
        connected_rook_bonus[phase] = read();

        doubled_pawn_penalty[phase] = read();
        isolated_pawn_penalty[phase] = read();
    }

    file.close();
}

void Weights::save(const std::string& path) const
{
    std::ofstream file(path, std::ios::binary);

    auto write = [&file](int64_t value) {
        int16_t temp = static_cast<int16_t>(value);
        file.write(reinterpret_cast<char*>(&temp), sizeof(int16_t));
    };

    if (!file.is_open())
        throw std::runtime_error(
                "Could not open file " + path);

    for (PieceKind piece = KNIGHT; piece != KING; ++piece)
        write(piece_weights[piece]);

    for (GamePhase phase : {MIDDLE_GAME, END_GAME})
    {
        for (PieceKind piece = PAWN; piece != KING; ++piece)
        {
            write(piece_values[phase][piece]);
            for (Square square = SQ_A1; square <= SQ_H8; ++square)
                write(piece_positional_values[phase][piece][square]);
        }

        for (Square square = SQ_A1; square <= SQ_H8; ++square)
            write(piece_positional_values[phase][KING][square]);

        for (PieceKind piece = PAWN; piece <= KING; ++piece)
            write(mobility_bonus[phase][piece]);

        write(rook_semiopen_file_bonus[phase]);
        write(rook_open_file_bonus[phase]);
        write(bishop_pair_bonus[phase]);
        write(passed_pawn_bonus[phase]);
        write(connected_rook_bonus[phase]);

        write(doubled_pawn_penalty[phase]);
        write(isolated_pawn_penalty[phase]);
    }

    file.close();
}

Weights load(const std::string& path)
{
    return Weights(path);
}

}
