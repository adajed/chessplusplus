#ifndef CHESS_ENGINE_MOVE_PICKER_H_
#define CHESS_ENGINE_MOVE_PICKER_H_

#include "position.h"
#include "types.h"

#include <vector>

namespace engine
{

class MovePicker
{
    public:
        MovePicker(const Position& position, Move* begin, Move* end);

        bool has_next();

        Move get_next();

    private:

        uint32_t score_move(const Position& position, Move move);

        Move* begin;
        Move* end;
        size_t pos;
        std::vector<uint32_t> scores;
};

}  // namespace engine

#endif  // CHESS_ENGINE_MOVE_PICKER_H_
