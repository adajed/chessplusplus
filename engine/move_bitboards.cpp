#include "move_bitboards.h"

#include "bithacks.h"
#include "types.h"

namespace engine
{

Bitboard RAYS[RAYS_NUM][SQUARE_NUM];

Bitboard KNIGHT_MASK[SQUARE_NUM];
Bitboard BISHOP_MASK[SQUARE_NUM];
Bitboard ROOK_MASK[SQUARE_NUM];
Bitboard KING_MASK[SQUARE_NUM];

uint64_t BISHOP_MAGICS[SQUARE_NUM] = {
    0x89a1121896040240ULL, 0x2004844802002010ULL, 0x2068080051921000ULL, 0x62880a0220200808ULL, 0x4042004000000ULL,
    0x100822020200011ULL, 0xc00444222012000aULL, 0x28808801216001ULL, 0x400492088408100ULL, 0x201c401040c0084ULL,
    0x840800910a0010ULL, 0x82080240060ULL, 0x2000840504006000ULL, 0x30010c4108405004ULL, 0x1008005410080802ULL,
    0x8144042209100900ULL, 0x208081020014400ULL, 0x4800201208ca00ULL, 0xf18140408012008ULL, 0x1004002802102001ULL,
    0x841000820080811ULL, 0x40200200a42008ULL, 0x800054042000ULL, 0x88010400410c9000ULL, 0x520040470104290ULL,
    0x1004040051500081ULL, 0x2002081833080021ULL, 0x400c00c010142ULL, 0x941408200c002000ULL, 0x658810000806011ULL,
    0x188071040440a00ULL, 0x4800404002011c00ULL, 0x104442040404200ULL, 0x511080202091021ULL, 0x4022401120400ULL,
    0x80c0040400080120ULL, 0x8040010040820802ULL, 0x480810700020090ULL, 0x102008e00040242ULL, 0x809005202050100ULL,
    0x8002024220104080ULL, 0x431008804142000ULL, 0x19001802081400ULL, 0x200014208040080ULL, 0x3308082008200100ULL,
    0x41010500040c020ULL, 0x4012020c04210308ULL, 0x208220a202004080ULL, 0x111040120082000ULL, 0x6803040141280a00ULL,
    0x2101004202410000ULL, 0x8200000041108022ULL, 0x21082088000ULL, 0x2410204010040ULL, 0x40100400809000ULL,
    0x822088220820214ULL, 0x40808090012004ULL, 0x910224040218c9ULL, 0x402814422015008ULL, 0x90014004842410ULL,
    0x1000042304105ULL, 0x10008830412a00ULL, 0x2520081090008908ULL, 0x40102000a0a60140ULL,
};

uint64_t ROOK_MAGICS[SQUARE_NUM] = {
    0xa8002c000108020ULL, 0x6c00049b0002001ULL, 0x100200010090040ULL, 0x2480041000800801ULL, 0x280028004000800ULL,
    0x900410008040022ULL, 0x280020001001080ULL, 0x2880002041000080ULL, 0xa000800080400034ULL, 0x4808020004000ULL,
    0x2290802004801000ULL, 0x411000d00100020ULL, 0x402800800040080ULL, 0xb000401004208ULL, 0x2409000100040200ULL,
    0x1002100004082ULL, 0x22878001e24000ULL, 0x1090810021004010ULL, 0x801030040200012ULL, 0x500808008001000ULL,
    0xa08018014000880ULL, 0x8000808004000200ULL, 0x201008080010200ULL, 0x801020000441091ULL, 0x800080204005ULL,
    0x1040200040100048ULL, 0x120200402082ULL, 0xd14880480100080ULL, 0x12040280080080ULL, 0x100040080020080ULL,
    0x9020010080800200ULL, 0x813241200148449ULL, 0x491604001800080ULL, 0x100401000402001ULL, 0x4820010021001040ULL,
    0x400402202000812ULL, 0x209009005000802ULL, 0x810800601800400ULL, 0x4301083214000150ULL, 0x204026458e001401ULL,
    0x40204000808000ULL, 0x8001008040010020ULL, 0x8410820820420010ULL, 0x1003001000090020ULL, 0x804040008008080ULL,
    0x12000810020004ULL, 0x1000100200040208ULL, 0x430000a044020001ULL, 0x280009023410300ULL, 0xe0100040002240ULL,
    0x200100401700ULL, 0x2244100408008080ULL, 0x8000400801980ULL, 0x2000810040200ULL, 0x8010100228810400ULL,
    0x2000009044210200ULL, 0x4080008040102101ULL, 0x40002080411d01ULL, 0x2005524060000901ULL, 0x502001008400422ULL,
    0x489a000810200402ULL, 0x1004400080a13ULL, 0x4000011008020084ULL, 0x26002114058042ULL
};

int BISHOP_INDEX_BITS[SQUARE_NUM] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};
int ROOK_INDEX_BITS[SQUARE_NUM] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

Bitboard BISHOP_TABLE[SQUARE_NUM][4096];
Bitboard ROOK_TABLE[SQUARE_NUM][4096];

Bitboard CASTLING_PATHS[1 << 4];
Bitboard LINES[SQUARE_NUM][SQUARE_NUM];

Castling CASTLING_RIGHTS[COLOR_NUM] = {W_CASTLING, B_CASTLING};
Square KING_SIDE_ROOK_SQUARE[COLOR_NUM] = {SQ_H1, SQ_H8};
Square QUEEN_SIDE_ROOK_SQUARE[COLOR_NUM] = {SQ_A1, SQ_A8};

Bitboard QUEEN_CASTLING_BLOCK[COLOR_NUM] = {square_bb(SQ_B1), square_bb(SQ_B8)};


namespace
{

Bitboard get_blockers_from_index(int index, Bitboard mask)
{
    Bitboard blockers = 0ULL;
    int num_bits = popcount(mask);
    for (int i = 0; i < num_bits; ++i)
    {
        Square position = Square(pop_lsb(&mask));
        if (index & (1 << i))
            blockers |= square_bb(position);
    }
    return blockers;
}

Bitboard get_attack_in_ray(Square sq, RayDirection ray, Bitboard blockers)
{
    Bitboard masked_blockers = blockers & RAYS[ray][sq];
    if (!masked_blockers)
        return RAYS[ray][sq];
    Square blocker;
    if (RAY_NW <= ray && ray < RAY_SE)
        blocker = Square(lsb(masked_blockers));
    else
        blocker = Square(msb(masked_blockers));

    return RAYS[ray][sq] & ~RAYS[ray][blocker];
}

Bitboard get_bishop_attacks(Square from, Bitboard blockers)
{
    Bitboard bb = 0ULL;
    bb |= get_attack_in_ray(from, RAY_NW, blockers);
    bb |= get_attack_in_ray(from, RAY_NE, blockers);
    bb |= get_attack_in_ray(from, RAY_SW, blockers);
    bb |= get_attack_in_ray(from, RAY_SE, blockers);
    return bb;
}

Bitboard get_rook_attacks(Square from, Bitboard blockers)
{
    Bitboard bb = 0ULL;
    bb |= get_attack_in_ray(from, RAY_N, blockers);
    bb |= get_attack_in_ray(from, RAY_E, blockers);
    bb |= get_attack_in_ray(from, RAY_S, blockers);
    bb |= get_attack_in_ray(from, RAY_W, blockers);
    return bb;
}

void init_rays()
{
    Direction directions[] = {
        NORTHWEST, NORTH, NORTHEAST, EAST,
        SOUTHEAST, SOUTH, SOUTHWEST, WEST
    };

    for (int i = 0; i < 8; ++i)
    {
        Direction dir = directions[i];
        for (Square sq = SQ_A1; sq <= SQ_H8; ++sq)
        {
            Bitboard bb = 0ULL;
            Bitboard field = shift(square_bb(sq), dir);
            while (field)
            {
                bb |= field;
                field = shift(field, dir);
            }
            RAYS[i][sq] = bb;
        }
    }
}

void init_knight_mask()
{
    for (Square sq = SQ_A1; sq <= SQ_H8; ++sq)
    {
        Bitboard bb = 0ULL;
        bb |= shift<NORTH>(shift<NORTH>(shift<EAST>(square_bb(sq))));
        bb |= shift<NORTH>(shift<NORTH>(shift<WEST>(square_bb(sq))));
        bb |= shift<SOUTH>(shift<SOUTH>(shift<EAST>(square_bb(sq))));
        bb |= shift<SOUTH>(shift<SOUTH>(shift<WEST>(square_bb(sq))));
        bb |= shift<EAST>(shift<EAST>(shift<NORTH>(square_bb(sq))));
        bb |= shift<EAST>(shift<EAST>(shift<SOUTH>(square_bb(sq))));
        bb |= shift<WEST>(shift<WEST>(shift<NORTH>(square_bb(sq))));
        bb |= shift<WEST>(shift<WEST>(shift<SOUTH>(square_bb(sq))));
        KNIGHT_MASK[sq] = bb;
    }
}

void init_bishop_mask()
{
    for (Square sq = SQ_A1; sq <= SQ_H8; ++sq)
    {
        Bitboard bb = 0ULL;
        bb |= RAYS[0][sq];
        bb |= RAYS[2][sq];
        bb |= RAYS[4][sq];
        bb |= RAYS[6][sq];
        BISHOP_MASK[sq] = bb & ~(fileA_bb | fileH_bb | rank1_bb | rank8_bb);
    }
}

void init_rook_mask()
{
    for (Square sq = SQ_A1; sq <= SQ_H8; ++sq)
    {
        Bitboard bb = 0ULL;
        bb |= RAYS[1][sq] & ~rank8_bb;
        bb |= RAYS[3][sq] & ~fileH_bb;
        bb |= RAYS[5][sq] & ~rank1_bb;
        bb |= RAYS[7][sq] & ~fileA_bb;
        ROOK_MASK[sq] = bb;
    }
}

void init_king_mask()
{
    for (Square sq = SQ_A1; sq <= SQ_H8; ++sq)
    {
        Bitboard bb = 0ULL;
        bb |= shift<NORTH>(square_bb(sq));
        bb |= shift<SOUTH>(square_bb(sq));
        bb |= shift<EAST>(square_bb(sq));
        bb |= shift<WEST>(square_bb(sq));
        bb |= shift<NORTHEAST>(square_bb(sq));
        bb |= shift<NORTHWEST>(square_bb(sq));
        bb |= shift<SOUTHEAST>(square_bb(sq));
        bb |= shift<SOUTHWEST>(square_bb(sq));
        KING_MASK[sq] = bb;
    }
}

void init_bishop_magics()
{
    for (Square sq = SQ_A1; sq <= SQ_H8; ++sq)
    {
        for (int i = 0; i < 4096; ++i)
            BISHOP_TABLE[sq][i] = all_squares_bb;
        int num_blockers = 1 << BISHOP_INDEX_BITS[sq];
        for (int index = 0; index < num_blockers; ++index)
        {
            Bitboard blockers = get_blockers_from_index(index, BISHOP_MASK[sq]);
            uint64_t key = (blockers * BISHOP_MAGICS[sq]) >> (64 - BISHOP_INDEX_BITS[sq]);
            Bitboard moves = get_bishop_attacks(sq, blockers);
            if (BISHOP_TABLE[sq][key] != all_squares_bb)
                assert(BISHOP_TABLE[sq][key] == moves);
            else
                BISHOP_TABLE[sq][key] = moves;
        }
    }
}

void init_rook_magics()
{
    for (Square sq = SQ_A1; sq <= SQ_H8; ++sq)
    {
        for (int i = 0; i < 4096; ++i)
            ROOK_TABLE[sq][i] = all_squares_bb;
        int num_blockers = 1 << ROOK_INDEX_BITS[sq];
        for (int index = 0; index < num_blockers; ++index)
        {
            Bitboard blockers = get_blockers_from_index(index, ROOK_MASK[sq]);
            uint64_t key = (blockers * ROOK_MAGICS[sq]) >> (64 - ROOK_INDEX_BITS[sq]);
            Bitboard moves = get_rook_attacks(sq, blockers);
            if (ROOK_TABLE[sq][key] != all_squares_bb)
                assert(ROOK_TABLE[sq][key] == moves);
            else
                ROOK_TABLE[sq][key] = moves;
        }
    }
}


void init_castling_paths_bitboards()
{
    CASTLING_PATHS[W_OO]  = 0x0000000000000060ULL;
    CASTLING_PATHS[W_OOO] = 0x000000000000000CULL;
    CASTLING_PATHS[B_OO]  = 0x6000000000000000ULL;
    CASTLING_PATHS[B_OOO] = 0x0C00000000000000ULL;
}

void init_lines_bitboards()
{
    Direction directions[] = {
        NORTHWEST, NORTH, NORTHEAST, EAST,
        SOUTHEAST, SOUTH, SOUTHWEST, WEST
    };
    int moves[] = {7, 8, 9, 1, -7, -8, -9, -1};

    for (Square from = SQ_A1; from <= SQ_H8; ++from)
        for (Square to = SQ_A1; to <= SQ_H8; ++to)
            LINES[from][to] = 0ULL;

    for (Square from = SQ_A1; from <= SQ_H8; ++from)
    {
        for (int i = 0; i < 8; ++i)
        {
            Square to = from;
            Bitboard to_bb = square_bb(to);
            Bitboard bb = to_bb;

            while (to_bb)
            {
                LINES[from][to] = bb;
                bb |= shift(bb, directions[i]);
                to_bb = shift(to_bb, directions[i]);
                to = Square(to + moves[i]);
            }
        }
    }
}

}

void init_move_bitboards()
{
    init_rays();
    init_knight_mask();
    init_bishop_mask();
    init_rook_mask();
    init_king_mask();
    init_bishop_magics();
    init_rook_magics();
    init_castling_paths_bitboards();
    init_lines_bitboards();
}

}
