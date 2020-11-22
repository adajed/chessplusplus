#include "move_picker.h"

#include "types.h"

#include <algorithm>

namespace engine
{


// bonus for caputure
// first dim is PieceKind of captured piece
// second dim is PieceKind of capturing piece
const uint32_t CAPTURE_BONUS[PIECE_KIND_NUM][PIECE_KIND_NUM] = {
    {},
    {0, 6, 5, 4, 3, 2, 1},          // captured pawn
    {0, 12, 11, 10, 9, 8, 7},       // captured knight
    {0, 18, 17, 16, 15, 14, 13},    // captured bishop
    {0, 24, 23, 22, 21, 20, 19},    // captured rook
    {0, 30, 29, 28, 27, 26, 25},    // captured queen
    {}
};

// bonus for promotion
const uint32_t PROMOTION_BONUS[PIECE_KIND_NUM] = {
    0,
    0,
    1, // knight
    2, // bishop
    3, // rook
    4, // queen
    0};


MovePicker::MovePicker(const Position& position, Move* begin, Move* end, OrderingInfo& info, bool use_info)
    : _moves(end - begin), _pos(0)
{
    size_t n = end - begin;
    for (size_t i = 0; i < n; ++i)
        _moves[i] = std::make_pair(0, begin[i]);

    score_moves(position, info, use_info);

    std::sort(_moves.begin(), _moves.end(), std::greater<>());
}

bool MovePicker::has_next()
{
    return _pos < _moves.size();
}

Move MovePicker::get_next()
{
    return _moves[_pos++].second;
}

void MovePicker::score_moves(const Position& position, OrderingInfo& info, bool use_info)
{
    Move pvMove = NO_MOVE;
    if (use_info)
    {
        const tt::TTEntry* entryPtr = info.ttable.get(position.hash());
        if (entryPtr)
            pvMove = entryPtr->move;
    }
    for (uint32_t i = 0; i < _moves.size(); ++i)
    {
        Move move = _moves[i].second;

        PieceKind moved_piece = make_piece_kind(position.piece_at(from(move)));
        PieceKind captured_piece = make_piece_kind(position.piece_at(to(move)));
        PieceKind promotion_piece = promotion(move);

        if (move == pvMove)
            _moves[i].first = 1000000;
        else if (captured_piece != NO_PIECE_KIND)
            _moves[i].first = 30000 + CAPTURE_BONUS[captured_piece][moved_piece];
        else if (promotion_piece != NO_PIECE_KIND)
            _moves[i].first = 29000 + PROMOTION_BONUS[promotion_piece];
        else if (!use_info)
            _moves[i].first = 1;
        else if (move == info.killers[info.ply][0])
            _moves[i].first = 28000;
        else if (move == info.killers[info.ply][1])
            _moves[i].first = 27000;
        else
            _moves[i].first = 1 + info.history[position.side_to_move()][from(move)][to(move)];
    }
}

}  // namespace engine
