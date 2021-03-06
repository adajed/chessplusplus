#include "move_orderer.h"

#include "types.h"

namespace engine
{
// bonus for caputure
// first dim is PieceKind of captured piece
// second dim is PieceKind of capturing piece
const uint32_t CAPTURE_BONUS[PIECE_KIND_NUM][PIECE_KIND_NUM] = {
    {},
    {0, 6, 5, 4, 3, 2, 1},  // captured pawn
    {0, 12, 11, 10, 9, 8, 7},  // captured knight
    {0, 18, 17, 16, 15, 14, 13},  // captured bishop
    {0, 24, 23, 22, 21, 20, 19},  // captured rook
    {0, 30, 29, 28, 27, 26, 25},  // captured queen
    {}};

// bonus for promotion
const uint32_t PROMOTION_BONUS[PIECE_KIND_NUM] = {0, 0,
                                                  1,  // knight
                                                  2,  // bishop
                                                  3,  // rook
                                                  4,  // queen
                                                  0};

MoveOrderer::MoveOrderer(tt::TTable& ttable, HistoryScore& historyScore)
    : _scores(), _pos(0), _ttable(ttable), _history_score(historyScore)
{
}

void MoveOrderer::order_moves(const Position& position, Move* begin, Move* end,
                              Info* info)
{
    Move pvMove = NO_MOVE;
    const tt::TTEntry* entryPtr = _ttable.get(position.hash());
    if (entryPtr) pvMove = entryPtr->move;

    int n_moves = static_cast<int>(end - begin);

    // score moves
    for (uint32_t i = 0; i < n_moves; ++i)
    {
        Move move = begin[i];

        Piece moved_piece = position.piece_at(from(move));
        PieceKind captured_piece = make_piece_kind(position.piece_at(to(move)));
        PieceKind promotion_piece = promotion(move);

        if (move == pvMove)
            _scores[i] = 1000000;
        else if (captured_piece != NO_PIECE_KIND)
            _scores[i] = 30000 + CAPTURE_BONUS[captured_piece][make_piece_kind(moved_piece)];
        else if (promotion_piece != NO_PIECE_KIND)
            _scores[i] = 29000 + PROMOTION_BONUS[promotion_piece];
        else if (move == info->_killer_moves[0])
            _scores[i] = 28000;
        else if (move == info->_killer_moves[1])
            _scores[i] = 27000;
        else
        {
            int h = _history_score[position.color()][from(move)][to(move)];
            int c = (*(info-1)->_counter_move)[moved_piece][to(move)];
            _scores[i] = std::min(h + c, 26999);
        }
    }

    // insertion sort
    for (int i = 0; i < n_moves - 1; ++i)
    {
        int best = i;
        for (int j = i + 1; j < n_moves; ++j)
        {
            if (_scores[j] > _scores[best]) best = j;
        }

        if (best != i)
        {
            std::swap(_scores[i], _scores[best]);
            std::swap(begin[i], begin[best]);
        }
    }

#ifdef DEBUG
    std::cout << "Move order: ";
    for (int i = 0; i < n_moves; ++i)
        std::cout << "(" << position.uci(begin[i]) << "," << _scores[i] << ") ";
    std::cout << std::endl;
#endif
}

}  // namespace engine
