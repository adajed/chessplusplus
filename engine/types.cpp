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

std::ostream& print_move(std::ostream& stream, Move move)
{
    const std::string files = "abcdefgh";
    const std::string ranks = "12345678";
    const std::string promotions = "  NBRQ ";

    if (castling(move) & KING_CASTLING)
        return stream << "OO";
    if (castling(move) & QUEEN_CASTLING)
        return stream << "OOO";

    stream << files[file(from(move))] << ranks[rank(from(move))]
           << files[file(to(move))]   << ranks[rank(to(move))];

    if (promotion(move) != NO_PIECE_KIND)
        stream << promotions[promotion(move)];

    return stream;
}

std::ostream& print_bitboard(std::ostream& stream, Bitboard bb)
{
    stream << "##########" << std::endl;
    for (int rank = 7; rank >= 0; rank--)
    {
        stream << "#";
        for (int file = 0; file < 8; ++file)
        {
            if (bb & square_bb(make_square(Rank(rank), File(file))))
                stream << ".";
            else
                stream << " ";
        }
        stream << "#" << std::endl;
    }
    stream << "##########" << std::endl;
    return stream;
}

}
