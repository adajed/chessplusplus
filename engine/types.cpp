#include "types.h"

namespace engine
{

Bitboard RANKS_BB[RANK_NUM] = {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8
};

Bitboard FILES_BB[FILE_NUM] = {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H
};

std::ostream& operator<< (std::ostream& stream, const Move& move)
{
    const std::string files = "abcdefgh";
    const std::string ranks = "12345678";

    if (move.castling & KING_CASTLING)
        return stream << "OO";
    else if (move.castling & QUEEN_CASTLING)
        return stream << "OOO";

    stream << files[file(move.from)] << ranks[rank(move.from)] << "-"
           << files[file(move.to)] << ranks[rank(move.to)];

    if (move.promotion == KNIGHT)
        return stream << "-N";
    if (move.promotion == BISHOP)
        return stream << "-B";
    if (move.promotion == ROOK)
        return stream << "-R";
    if (move.promotion == QUEEN)
        return stream << "-Q";

    return stream;

}

}
