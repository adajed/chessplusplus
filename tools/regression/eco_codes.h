#pragma once

#include "types.h"
#include <vector>

class ExtendedPosition;

namespace eco
{

struct Code
{
    std::vector<engine::Move> moveList;
    std::string code;
    std::string openingName;
    std::string fen;
};

void init();

Code get(const ExtendedPosition& position);

void printAllCodes();

}  // namespace eco
