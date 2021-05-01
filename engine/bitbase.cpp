#include "bitboard.h"
#include "bithacks.h"
#include "endgame.h"
#include "types.h"
#include <bits/stdint-uintn.h>
#include <cassert>
#include <math.h>
#include <vector>

namespace engine
{

namespace bitbase
{

constexpr int MAX_INDEX = 2 * 24 * 64 * 64;

uint32_t BITBASE[MAX_INDEX / 32];

// index:
// 0-5   : white king
// 6-11  : black king
// 12    : color
// 13-14 : pawn file
// 15-17 : pawn rank
uint32_t getIndex(Color side, Square wKing, Square wPawn, Square bKing)
{
    assert(file(wPawn) <= FILE_D);
    assert(RANK_2 <= rank(wPawn) && rank(wPawn) <= RANK_7);
    uint32_t index = wKing | (bKing << 6) | (side << 12) | (file(wPawn) << 13) | ((rank(wPawn) - RANK_2) << 15);
    assert(index < MAX_INDEX);
    return index;
}

void parse_index(uint32_t idx, Color& side, Square& wKing, Square& wPawn, Square& bKing)
{
    assert(idx < MAX_INDEX);
    wKing = Square(idx & 0x3F);
    bKing = Square((idx >> 6) & 0x3F);
    side = Color((idx >> 12) & 0x1);
    wPawn = make_square(Rank(((idx >> 15) & 0x7) + RANK_2), File((idx >> 13) & 0x3));
}

bool check(Color side, Square wKing, Square wPawn, Square bKing)
{
    uint32_t idx = getIndex(side, wKing, wPawn, bKing);
    return BITBASE[idx / 32] & (1 << (idx & 0x1F));
}


enum Result : uint32_t
{
    kINVALID = 0,
    kUNKNOWN = 1,
    kWIN = 2,
    kDRAW = 3
};

Result initial_score(uint32_t idx)
{
    Color side;
    Square wKing, wPawn, bKing;
    parse_index(idx, side, wKing, wPawn, bKing);

    bool blackInCheck = bool(pawn_attacks<WHITE>(square_bb(wPawn)) & square_bb(bKing));

    // kings cannot stand on the same sqaure or next to each other
    // pawn cannot stand on the same square as any king
    // if white to move, black king cannot be in check
    if (distance(wKing, bKing) <= 1
            || wKing == wPawn
            || bKing == wPawn
            || (side == WHITE && blackInCheck))
        return kINVALID;

    Bitboard bKingMoves = king_attacks(square_bb(bKing)) & (~king_attacks(square_bb(wKing))) & (~pawn_attacks<WHITE>(square_bb(wPawn)));

    if (side == BLACK && !bKingMoves)
        return kDRAW;

    Square nextPawnSquare = make_square(Rank(rank(wPawn) + 1), file(wPawn));

    if (side == WHITE
            && rank(wPawn) == RANK_7
            && wKing != nextPawnSquare
            && bKing != nextPawnSquare
            && !bool(bKingMoves & square_bb(nextPawnSquare)))
        return kWIN;

    if (side == BLACK
            && bool(bKingMoves & square_bb(wPawn)))
        return kDRAW;

    return kUNKNOWN;
}

Result update_score(const std::vector<Result>& results, uint32_t idx)
{
    Color side;
    Square wKing, wPawn, bKing;
    parse_index(idx, side, wKing, wPawn, bKing);

    Result betterResult = side == WHITE ? kWIN : kDRAW;
    Result worseResult  = side == WHITE ? kDRAW : kWIN;

    Square ourKing = side == WHITE ? wKing : bKing;


    bool isUnknown = false;

    Bitboard kingMoves = king_attacks(square_bb(ourKing));
    while (kingMoves)
    {
        Square nextKingSq = Square(pop_lsb(&kingMoves));

        uint32_t index = getIndex(!side,
                                  side == WHITE ? nextKingSq : wKing,
                                  wPawn,
                                  side == WHITE ? bKing : nextKingSq);

        if (results[index] == betterResult)
            return betterResult;

        isUnknown |= (results[index] == kUNKNOWN);
    }

    // if white side, check pawn moves
    if (side == WHITE && rank(wPawn) != RANK_7)
    {
        // single push
        Square nextPawnSq = make_square(Rank(rank(wPawn) + 1), file(wPawn));
        uint32_t index = getIndex(BLACK, wKing, nextPawnSq, bKing);
        if (results[index] == betterResult)
            return betterResult;
        isUnknown |= (results[index] == kUNKNOWN);

        // double push if pawn is on RANK_2
        if (rank(wPawn) == RANK_2)
        {
            nextPawnSq = make_square(RANK_4, file(wPawn));
            uint32_t index = getIndex(BLACK, wKing, nextPawnSq, bKing);
            if (results[index] == betterResult)
                return betterResult;
            isUnknown |= (results[index] == kUNKNOWN);
        }
    }

    return isUnknown ? kUNKNOWN : worseResult;
}

void init()
{
    for (uint32_t i = 0; i < MAX_INDEX / 32; ++i)
        BITBASE[i] = 0;

    std::vector<Result> results(MAX_INDEX, kUNKNOWN);

    for (uint32_t idx = 0; idx < MAX_INDEX; ++idx)
        results[idx] = initial_score(idx);

    bool repeat = true;
    while (repeat)
    {
        repeat = false;
        for (uint32_t idx = 0; idx < MAX_INDEX; ++idx)
        {
            if (results[idx] == kUNKNOWN)
            {
                results[idx] = update_score(results, idx);
                repeat |= (results[idx] != kUNKNOWN);
            }
        }
    }

    for (uint32_t idx = 0; idx < MAX_INDEX; ++idx)
        if (results[idx] == kWIN)
            BITBASE[idx / 32] |= (1 << (idx & 0x1F));
}

void normalize(Color strongSide, Color& side, Square& strongKing, Square& strongPawn, Square& weakKing)
{
    // flip everything so pawn is on files A-D
    if (file(strongPawn) > FILE_D)
    {
        strongKing = flip_horizontally(strongKing);
        strongPawn = flip_horizontally(strongPawn);
        weakKing = flip_horizontally(weakKing);
    }

    // flip everything such that strong side is WHITE
    if (strongSide == BLACK)
    {
        strongKing = flip_vertically(strongKing);
        strongPawn = flip_vertically(strongPawn);
        weakKing = flip_vertically(weakKing);
        side = !side;
    }
}

}

}
