#ifndef CHESS_ENGINE_WEIGHTS_H_
#define CHESS_ENGINE_WEIGHTS_H_

#include <cinttypes>
#include <string>

#include "types.h"

namespace engine
{

struct Weights
{
    int32_t piece_values[GAME_PHASE_NUM][PIECE_KIND_NUM];
    int32_t piece_positional_values[GAME_PHASE_NUM][PIECE_KIND_NUM][SQUARE_NUM];
    int32_t mobility_bonus[GAME_PHASE_NUM][PIECE_KIND_NUM];
    int32_t rook_semiopen_file_bonus[GAME_PHASE_NUM];
    int32_t rook_open_file_bonus[GAME_PHASE_NUM];
    int32_t bishop_pair_bonus[GAME_PHASE_NUM];
    int32_t passed_pawn_bonus[GAME_PHASE_NUM];

    int32_t doubled_pawn_penalty[GAME_PHASE_NUM];
    int32_t isolated_pawn_penalty[GAME_PHASE_NUM];
};

Weights load(const std::string& path);

/* void save(const Weights& weights, const std::string& path); */

}

#endif  // CHESS_ENGINE_WEIGHTS_H_
