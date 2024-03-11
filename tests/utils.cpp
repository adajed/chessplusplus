#include "utils.h"

namespace test_utils
{

PositionGenerator::PositionGenerator(const std::vector<Piece>& pieces)
    : pieces_(pieces)
    , piece_squares_(pieces.size(), SQ_A1)
    , next_position_()
    , has_next_(false)
{
    prepare_next_position();
}

bool PositionGenerator::hasNext() const
{
    return has_next_;
}

Position PositionGenerator::next()
{
    Position p = next_position_;
    prepare_next_position();
    return p;
}

bool PositionGenerator::inc_squares()
{
    for (size_t i = 0; i < piece_squares_.size(); ++i)
    {
        if (piece_squares_[i] < SQ_H8)
        {
            ++piece_squares_[i];
            return true;
        }
        piece_squares_[i] = SQ_A1;
    }
    return false;
}

bool PositionGenerator::has_repeats() const
{
    for (size_t i = 1; i < piece_squares_.size(); ++i)
    {
        for (size_t j = 0; j < i; ++j)
        {
            if (piece_squares_[i] == piece_squares_[j])
                return true;
        }
    }
    return false;
}

void PositionGenerator::prepare_next_position()
{
    do
    {
        do
        {
            has_next_ = inc_squares();
            if (!has_next_) return;
        } while (has_repeats());

        std::vector<std::pair<Piece, Square>> pieces;
        for (size_t i = 0; i < pieces_.size(); ++i)
        {
            pieces.push_back(std::make_pair(pieces_[i], piece_squares_[i]));
        }
        next_position_ = Position(pieces);
    } while (!next_position_.is_legal());
}


}  // namespace test_utils
