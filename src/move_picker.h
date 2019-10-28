#ifndef CHESS_ENGINE_MOVE_PICKER_H_
#define CHESS_ENGINE_MOVE_PICKER_H_

#include "position.h"
#include "types.h"

#include <vector>

namespace engine
{

struct OrderingInfo
{
    public:
        OrderingInfo()
        {
            for (int i = 0; i < 50; ++i)
                killers[i][0] = killers[i][1] = NO_MOVE;
            for (int c = 0; c < 2; ++c)
                for (int from = 0; from < SQUARE_NUM; ++from)
                    for (int to = 0; to < SQUARE_NUM; ++to)
                        history[c][from][to] = 0;
            ply = 0;
        }

        void update_killers(int p, Move move)
        {
            killers[p][1] = killers[p][0];
            killers[p][0] = move;
        }

        void update_history(Color c, Square from, Square to, int depth)
        {
            history[c][from][to] += depth * depth;
        }

        Move killers[50][2];
        int history[COLOR_NUM][SQUARE_NUM][SQUARE_NUM];
        int ply;
};


class MovePicker
{
    public:
        MovePicker(const Position& position, Move* begin, Move* end, OrderingInfo& info, bool use_info);

        bool has_next();

        Move get_next();

    private:

        uint32_t score_move(const Position& position, Move move, OrderingInfo& info, bool use_info);

        Move* begin;
        Move* end;
        size_t pos;
        std::vector<uint32_t> scores;
};

}  // namespace engine

#endif  // CHESS_ENGINE_MOVE_PICKER_H_
