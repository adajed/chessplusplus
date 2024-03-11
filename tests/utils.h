#pragma once

#include "position.h"
#include <vector>

using namespace engine;

namespace test_utils
{

class PositionGenerator
{
public:
    PositionGenerator(const std::vector<Piece>& pieces);

    bool hasNext() const;

    Position next();

private:
    /*
     * Increments vector of inc_squares.
     * Makes sure that squares don't repeat.
     * Returns true iff there is next combination.
     */
    bool inc_squares();

    bool has_repeats() const;

    void prepare_next_position();

    std::vector<Piece> pieces_;
    std::vector<Square> piece_squares_;
    Position next_position_;
    bool has_next_;
};

}
