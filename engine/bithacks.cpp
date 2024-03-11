#include "bithacks.h"

/*
 * All of the bithacks ideas (with support for different OS)
 * are heavily inspired from Stockfish code
 * (https://github.com/official-stockfish/Stockfish/blob/master/src/bitboard.h).
 */

#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace engine
{
int lsb(Bitboard bb)
{
#if defined(__GNUC__)
    return __builtin_ffsll(bb) - 1;
#elif defined(_MSC_VER)
    unsigned long index = 0;
#if defined(_WIN64)
    _BitScanForward64(&index, bb);
    return static_cast<int>(index);
#else  // MSVC, _WIN32
    if (bb & 0xFFFFFFFF)
    {
        _BitScanForward(&index, static_cast<unsigned long>(bb));
        return static_cast<int>(index);
    }
    else
    {
        _BitScanForward(&index, static_cast<unsigned long>(bb >> 32));
        return static_cast<int>(index) + 32;
    }
#endif
#else
#error "lsb: Compiler not supported."
#endif
}

int msb(Bitboard bb)
{
#if defined(__GNUC__)
    return 63 - __builtin_clzll(bb);
#elif defined(_MSC_VER)
    unsigned long index = 0;
#if defined(_WIN64)
    _BitScanReverse64(&index, bb);
    return static_cast<int>(index);
#else  // MSVC, _WIN32
    if (bb >> 32)
    {
        _BitScanReverse(&index, static_cast<unsigned long>(bb >> 32));
        return static_cast<int>(index) + 32;
    }
    else
    {
        _BitScanReverse(&index, static_cast<unsigned long>(bb));
        return static_cast<int>(index);
    }
#endif
#else
#error "msb: Compiler not supported."
#endif
}

int popcount(Bitboard bb)
{
#if defined(__GNUC__)
    return __builtin_popcountll(bb);
#elif defined(_MSC_VER)
#if defined(_WIN64)
    return __popcnt64(bb);
#else  // MSVC, _WIN32
    return __popcnt(static_cast<unsigned int>(bb >> 32)) +
           __popcnt(static_cast<unsigned int>(bb));
#endif
#else
#error "popcount: Compiler not supported"
#endif
}

uint32_t pop_lsb(Bitboard* bb)
{
    int bit = lsb(*bb);
    *bb &= *bb - 1;  // pop lsb
    return bit;
}

bool popcount_more_than_one(Bitboard bb)
{
    return bb && (bb & (bb - 1));
}

}  // namespace engine
