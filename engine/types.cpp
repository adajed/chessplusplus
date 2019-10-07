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

void print_move(Move move)
{
    const std::string files = "abcdefgh";
    const std::string ranks = "12345678";

    if (castling(move) & KING_CASTLING)
    {
        std::cout << "OO";
        return;
    }
    else if (castling(move) & QUEEN_CASTLING)
    {
        std::cout << "OOO";
        return;
    }

    std::cout << files[file(from(move))] << ranks[rank(from(move))]
           << files[file(to(move))] << ranks[rank(to(move))];

    if (promotion(move) == KNIGHT)
        std::cout << "N";
    if (promotion(move) == BISHOP)
        std::cout << "B";
    if (promotion(move) == ROOK)
        std::cout << "R";
    if (promotion(move) == QUEEN)
        std::cout << "Q";
}

}
