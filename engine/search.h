#ifndef CHESS_ENGINE_SEARCH_H_
#define CHESS_ENGINE_SEARCH_H_

#include "position.h"
#include "score.h"
#include "types.h"

#include <chrono>

namespace engine
{

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;


using Score = int64_t;

constexpr Score INFINITY_SCORE = 1LL << 32;
constexpr Score DRAW_SCORE = 0LL;

Score mate_in(int ply);
bool is_score_mate(Score score);


class Search
{
    public:
        explicit Search(const PositionScorer& scorer);

        Move select_move(const Position& position, int depth);

        void set_thinking_time(int64_t time);

        int64_t get_thinking_time();

        void stop();

    private:
        Score search(Position& position, int depth, Score alpha, Score beta, Move* pv);

        Score quiescence_search(Position& position, int depth, Score alpha, Score beta);

        bool check_limits();


        int64_t check_limits_counter;
        bool stop_search;

        PositionScorer scorer;

        TimePoint start_time;
        int64_t thinking_time;

        Move pv_moves[MAX_DEPTH];
};

}

#endif  // CHESS_ENGINE_SEARCH_H_
