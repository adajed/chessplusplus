#include "move_orderer.h"

#include "types.h"

namespace engine
{

constexpr MoveScore PV_SCORE = 2'000'000;
constexpr MoveScore TT_SCORE = PV_SCORE - 1;
constexpr MoveScore CAPTURE_SCORE = TT_SCORE - 50;
constexpr MoveScore PROMOTION_SCORE = CAPTURE_SCORE - 10;
constexpr MoveScore KILLER_1_SCORE = PROMOTION_SCORE - 1;
constexpr MoveScore KILLER_2_SCORE = KILLER_1_SCORE - 1;
constexpr MoveScore MAX_QUIET_SCORE = KILLER_2_SCORE - 1;

static_assert(TT_SCORE < PV_SCORE);
static_assert(CAPTURE_SCORE < TT_SCORE);
static_assert(PROMOTION_SCORE < CAPTURE_SCORE);
static_assert(KILLER_1_SCORE < PROMOTION_SCORE);
static_assert(KILLER_2_SCORE < KILLER_1_SCORE);
static_assert(MAX_QUIET_SCORE < KILLER_2_SCORE);

std::string moveScoreToString(MoveScore score)
{
    if (score == PV_SCORE) return "pv";
    if (score == TT_SCORE) return "tt";
    if (score >= CAPTURE_SCORE)
        return "capture+" + std::to_string(score - CAPTURE_SCORE);
    if (score >= PROMOTION_SCORE)
        return "promotion+" + std::to_string(score - PROMOTION_SCORE);
    if (score == KILLER_1_SCORE) return "killer1";
    if (score == KILLER_2_SCORE) return "killer2";
    return std::to_string(score);
}

// bonus for capture
// first dim is PieceKind of captured piece
// second dim is PieceKind of capturing piece
/**
 * @brief Bonus to order captures correctly.
 * First consider captures where we gain material.
 * Second consider captures of equal value.
 * Third consider captures where we lose material.
 * Inside each case we order by MVV-LVA
 */
const int CAPTURE_BONUS[PIECE_KIND_NUM][PIECE_KIND_NUM] = {
    {},
    //   p,  n,  b,  r,  q,  k
    {0, 14,  4,  3,  2,  1,  0},  // captured pawn
    {0, 21, 16, 15,  7,  6,  5},  // captured knight
    {0, 22, 18, 17, 10,  9,  8},  // captured bishop
    {0, 25, 24, 23, 19, 12, 11},  // captured rook
    {0, 29, 28, 27, 26, 20, 13},  // captured queen
    {}};

/**
 * @brief Orders PieceKind by most value.
 */
constexpr int mostValue(PieceKind pieceKind)
{
    return static_cast<int>(pieceKind);
}

static_assert(mostValue(KING) == mostValue(QUEEN) + 1);
static_assert(mostValue(QUEEN) == mostValue(ROOK) + 1);
static_assert(mostValue(ROOK) == mostValue(BISHOP) + 1);
static_assert(mostValue(BISHOP) == mostValue(KNIGHT) + 1);
static_assert(mostValue(KNIGHT) == mostValue(PAWN) + 1);

/**
 * @brief Orders PieceKind by least value.
 */
constexpr int leastValue(PieceKind pieceKind)
{
    return mostValue(KING) - mostValue(pieceKind) + 1;
}

static_assert(leastValue(PAWN) == leastValue(KNIGHT) + 1);
static_assert(leastValue(KNIGHT) == leastValue(BISHOP) + 1);
static_assert(leastValue(BISHOP) == leastValue(ROOK) + 1);
static_assert(leastValue(ROOK) == leastValue(QUEEN) + 1);
static_assert(leastValue(QUEEN) == leastValue(KING) + 1);

MoveOrderer::MoveOrderer(tt::TTable& ttable, HistoryScore& historyScore)
    : _scores(), _pos(0), _ttable(ttable), _history_score(historyScore)
{
}

void MoveOrderer::order_moves(const Position& position, Move* begin, Move* end,
                              Info* info)
{
    Move pvMove = info->_pv_list_length > 0 ? info->_pv_list[0] : NO_MOVE;
    Move ttMove = NO_MOVE;
    bool found = false;
    const auto entryPtr = _ttable.probe(position.hash(), found);
    if (found) ttMove = entryPtr->value.move;

    int n_moves = static_cast<int>(end - begin);

    // score moves
    int i = 0;
    for (Move* it = begin; it != end; ++it, ++i)
    {
        Move move = *it;
        _scores[i] = 0;

        Castling c = castling(move);
        Piece moved_piece = c == NO_CASTLING ? position.piece_at(from(move)) : NO_PIECE;
        PieceKind captured_piece = c == NO_CASTLING ? make_piece_kind(position.piece_at(to(move))) : NO_PIECE_KIND;
        PieceKind promotion_piece = promotion(move);

        if (move == pvMove)
            _scores[i] = PV_SCORE;
        else if (move == ttMove)
            _scores[i] = TT_SCORE;
        else if (captured_piece != NO_PIECE_KIND && promotion_piece == NO_PIECE_KIND)
        {
            // consider last move (re)captures first
            if (to(move) == to((info - 1)->_current_move))
            {
                _scores[i] = CAPTURE_SCORE + 40 + leastValue(make_piece_kind(moved_piece));
            }
            else
            {
                _scores[i] = CAPTURE_SCORE + CAPTURE_BONUS[captured_piece][make_piece_kind(moved_piece)];
            }
            ASSERT(_scores[i] < TT_SCORE);
        }
        else if (promotion_piece != NO_PIECE_KIND)
        {
            _scores[i] = PROMOTION_SCORE + mostValue(promotion_piece);
            ASSERT(_scores[i] < CAPTURE_SCORE);
        }
        else if (move == info->_killer_moves[0])
            _scores[i] = KILLER_1_SCORE;
        else if (move == info->_killer_moves[1])
            _scores[i] = KILLER_2_SCORE;
        else
        {
            int h = _history_score[position.color()][from(move)][to(move)];
            int c = (*(info-1)->_counter_move)[moved_piece][to(move)];
            _scores[i] = std::min(h + c, MAX_QUIET_SCORE);
            ASSERT(_scores[i] < KILLER_2_SCORE);
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

#if LOG_LEVEL > 1
    std::cerr << "[" << info->_ply << "] MOVE ORDER ";
    for (int i = 0; i < n_moves; ++i)
        std::cerr << "(" << position.uci(begin[i]) << "," << moveScoreToString(_scores[i]) << ") ";
    std::cerr << "\n";
#endif
}

}  // namespace engine
