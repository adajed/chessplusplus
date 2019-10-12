#ifndef CHESS_ENGINE_WEIGHTS_H_
#define CHESS_ENGINE_WEIGHTS_H_

#include <cinttypes>
#include <string>
#include <vector>

#include "engine.h"

namespace engine
{

class Weights
{
    public:
        Weights(std::string path);

        int32_t get_piece_value(PieceKind piecekind) const;

        int32_t get_piece_position_value(
                PieceKind piecekind, Square square) const;

        int32_t get_mobility_bonus() const;

        int32_t get_attacking_bonus() const;

    private:
        int32_t piece_values[PIECE_KIND_NUM];
        int32_t piece_position_values[PIECE_KIND_NUM][SQUARE_NUM];
        int32_t mobility_bonus;
        int32_t attacking_bonus;
};

}

#endif  // CHESS_ENGINE_WEIGHTS_H_
