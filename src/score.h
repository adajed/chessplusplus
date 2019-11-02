#ifndef CHESS_ENGINE_SCORE_H_
#define CHESS_ENGINE_SCORE_H_

#include "movegen.h"
#include "weights.h"

namespace engine
{

extern int64_t PIECE_BASE_VALUES[GAME_PHASE_NUM][PIECE_KIND_NUM];
extern int64_t PIECE_POSITIONAL_VALUES[GAME_PHASE_NUM][PIECE_KIND_NUM][SQUARE_NUM];
extern int64_t PIECE_MOBILITY_BONUS[GAME_PHASE_NUM][PIECE_KIND_NUM];
extern int64_t ROOK_SEMIOPEN_FILE_BONUS[GAME_PHASE_NUM];
extern int64_t ROOK_OPEN_FILE_BONUS[GAME_PHASE_NUM];
extern int64_t BISHOP_PAIR_BONUS[GAME_PHASE_NUM];
extern int64_t PASSED_PAWN_BONUS[GAME_PHASE_NUM];
extern int64_t DOUBLED_PAWN_PENALTY[GAME_PHASE_NUM];
extern int64_t ISOLATED_PAWN_PENALTY[GAME_PHASE_NUM];
extern int64_t CONNECTED_ROOKS_BONUS[GAME_PHASE_NUM];

class PositionScorer
{
    public:
        explicit PositionScorer();

        int64_t score(const Position& position);

    private:

        template <Color side>
        int64_t score_side(const Position& position);

        template <Color side, GamePhase phase>
        int64_t score(const Position& position);

        int64_t game_phase_weight(const Position& position);

        Move move_list[MAX_MOVES];
        int move_count[COLOR_NUM][PIECE_KIND_NUM];
};

}

#endif  // CHESS_ENGINE_SCORE_H_
