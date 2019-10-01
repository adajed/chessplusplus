#include "bithacks.h"

namespace engine
{

int lsb(Bitboard bb)
{
    return __builtin_ffsll(bb) - 1;
}

int msb(Bitboard bb)
{
    return 63 - __builtin_clzll(bb);
}

int popcount(Bitboard bb)
{
    return __builtin_popcountll(bb);
}

uint32_t pop_lsb(Bitboard* bb)
{
    int bit = lsb(*bb);
    *bb &= *bb - 1; // pop lsb
    return bit;
}


} // namespace engine
