#ifndef CHESS_ENGINE_SCORE_H_
#define CHESS_ENGINE_SCORE_H_

#include "movegen.h"
#include "pawn_hash_table.h"
#include "types.h"

namespace engine
{

struct Score
{
    Value mg, eg;

    Score() : mg(0), eg(0) {}
    Score(Value mg, Value eg) : mg(mg), eg(eg) {}
    explicit Score(Value v) : mg(v), eg(v) {}
    Score(const Score& other) = default;
    Score(Score&& other) = default;

    Score& operator= (const Score& other) = default;
    Score& operator= (Score&& other) = default;

    Score operator+ (const Score& other) const { return Score(mg + other.mg, eg + other.eg); }
    Score operator- (const Score& other) const { return Score(mg - other.mg, eg - other.mg); }

    Score& operator+= (const Score& other)
    {
        mg += other.mg;
        eg += other.eg;

        return *this;
    }
};

inline Score operator* (const Score& score, const Value& value)
{
    return Score(score.mg * value, score.eg * value);
}

inline Score operator* (const Value& value, const Score& score)
{
    return Score(score.mg * value, score.eg * value);
}


class PositionScorer
{
    public:
        PositionScorer();

        Value score(const Position& position);

        void clear();

    private:

        template <Color side>
        void setup(const Position& position);

        Score score_pieces(const Position& position);

        template <Color side>
        Score score_pieces_for_side(const Position& position);

        Score score_pawns(const Position& position);

        template <Color side>
        Score score_pawns_for_side(const Position& position);

        Value combine(const Score& score);

        Value game_phase_weight(const Position& position);

        Move move_list[MAX_MOVES];
        int move_count[COLOR_NUM][PIECE_KIND_NUM];

        PawnHashTable _pawn_hash_table;
        Value _weight;

        Bitboard _attacked_by_bb[COLOR_NUM][PIECE_KIND_NUM];
        Bitboard _attacked_by_piece[COLOR_NUM];
};

}

#endif  // CHESS_ENGINE_SCORE_H_
