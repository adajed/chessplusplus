#include "move_picker.h"

#include "types.h"

namespace engine
{

MovePicker::MovePicker(const Position& position, Move* begin, Move* end)
    : begin(begin), end(end), pos(0), scores(end - begin)
{
    size_t n = end - begin;
    for (size_t i = 0; i < n; ++i)
        scores[i] = score_move(position, begin[i]);
}

bool MovePicker::has_next()
{
    size_t n = end - begin;
    return pos < n;
}

Move MovePicker::get_next()
{
    size_t n = end - begin;
    uint32_t best_score = scores[pos];
    size_t best_index = pos;
    for (size_t i = pos + 1; i < n; ++i)
    {
        if (scores[i] > best_score)
        {
            best_score = scores[i];
            best_index = i;
        }
    }

    Move ret = begin[best_index];
    begin[best_index] = begin[pos];
    begin[pos] = ret;
    pos++;
    return ret;
}

uint32_t MovePicker::score_move(const Position& position, Move move)
{
    PieceKind moved_piece = make_piece_kind(position.board[from(move)]);
    PieceKind captured_piece = make_piece_kind(position.board[to(move)]);
    PieceKind promotion_piece = promotion(move);

    const uint32_t capture_bonus[PIECE_KIND_NUM][PIECE_KIND_NUM] = {
        {},
        {0, 6, 5, 4, 3, 2, 1},
        {0, 12, 11, 10, 9, 8, 7},
        {0, 18, 17, 16, 15, 14, 13},
        {0, 24, 23, 22, 21, 20, 19},
        {0, 30, 29, 28, 27, 26, 25},
        {}
    };

    const uint32_t promotion_bonus[PIECE_KIND_NUM] = {0, 0, 1, 2, 3, 4, 0};

    if (captured_piece != NO_PIECE_KIND)
        return 2000 + capture_bonus[captured_piece][moved_piece];
    if (promotion_piece != NO_PIECE_KIND)
        return 1000 + promotion_bonus[promotion_piece];
    return 1;
}

}  // namespace engine
