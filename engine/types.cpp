#include "types.h"
#include "bitboard.h"

namespace engine
{

Bitboard RANKS_BB[RANK_NUM] = {
    rank1_bb, rank2_bb, rank3_bb, rank4_bb, rank5_bb, rank6_bb, rank7_bb, rank8_bb
};

Bitboard FILES_BB[FILE_NUM] = {
    fileA_bb, fileB_bb, fileC_bb, fileD_bb, fileE_bb, fileF_bb, fileG_bb, fileH_bb
};

std::ostream& operator<< (std::ostream& stream, const Move& move)
{
    const std::string files = "abcdefgh";
    const std::string ranks = "12345678";

    if (move.castling & KING_CASTLING)
        return stream << "OO";
    else if (move.castling & QUEEN_CASTLING)
        return stream << "OOO";

    stream << files[file(move.from)] << ranks[rank(move.from)]
           << files[file(move.to)] << ranks[rank(move.to)];

    if (move.promotion == KNIGHT)
        return stream << "N";
    if (move.promotion == BISHOP)
        return stream << "B";
    if (move.promotion == ROOK)
        return stream << "R";
    if (move.promotion == QUEEN)
        return stream << "Q";

    return stream;

}

}
