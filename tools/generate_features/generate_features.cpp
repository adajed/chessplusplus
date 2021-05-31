#include "position.h"
#include "position_bitboards.h"
#include "endgame.h"
#include "movegen.h"
#include "zobrist_hash.h"

using namespace engine;

constexpr int NUM_FEATURES = 10 + 2 * (5 + 6 + 5 + 3) + 2 * 5 + 2 * 5;

template <Color side>
Rank relative_rank(Rank r)
{
    return side == WHITE ? r : RANK_8 - r;
}

class FeatureGenerator
{
public:
    FeatureGenerator(const std::string& fen)
        : position(fen)
    {
        setup<WHITE>();
        setup<BLACK>();
    }

    void generate(int* features)
    {
        *features++ = position.number_of_pieces(W_PAWN);
        *features++ = position.number_of_pieces(W_KNIGHT);
        *features++ = position.number_of_pieces(W_BISHOP);
        *features++ = position.number_of_pieces(W_ROOK);
        *features++ = position.number_of_pieces(W_QUEEN);
        *features++ = position.number_of_pieces(B_PAWN);
        *features++ = position.number_of_pieces(B_KNIGHT);
        *features++ = position.number_of_pieces(B_BISHOP);
        *features++ = position.number_of_pieces(B_ROOK);
        *features++ = position.number_of_pieces(B_QUEEN);

        features = generate_pieces<WHITE>(features);
        features = generate_pieces<BLACK>(features);
        features = generate_pawns<WHITE>(features);
        features = generate_pawns<BLACK>(features);
        features = generate_king<WHITE>(features);
        features = generate_king<BLACK>(features);
    }

private:

    template <Color side>
    int* generate_pieces(int* features)
    {
        const Square ownKing = position.piece_position(make_piece(side, KING), 0);
        const Square opponentsKing =
            position.piece_position(make_piece(!side, KING), 0);


        // knights
        {
            constexpr Piece piece = make_piece(side, KNIGHT);
            Bitboard opponent_pieces =
                position.pieces(!side) & ~position.pieces(!side, PAWN);
            int no_pieces = position.number_of_pieces(piece);

            int no_safe_knights = 0;
            int no_knight_control_center = 0;
            int no_knight_king_protector = 0;
            int no_knight_mobility = 0;
            int no_knight_outpost = 0;
            for (int i = 0; i < no_pieces; ++i)
            {
                Square sq = position.piece_position(piece, i);
                Bitboard attacking = KNIGHT_MASK[sq];

                if (square_bb(sq) & attacked_by[side][PAWN])
                {
                    no_safe_knights++;
                }
                no_knight_control_center += popcount(attacking & center_bb);

                no_knight_king_protector += distance(ownKing, sq);

                Bitboard moves = attacking;
                if (square_bb(sq) & blockers_for_king[side])
                {
                    moves &= FULL_LINES[sq][ownKing];
                }
                // cannot move into square with our piece
                moves &= ~position.pieces(side);
                // squares attacked by opponents pawns with nothing valueable there
                moves &= ~(attacked_by[!side][PAWN] & ~opponent_pieces);
                // attacked by opponents piece and not defended by our pawns
                moves &=
                    ~(attacked_by_piece[!side] & ~attacked_by[side][PAWN]);
                no_knight_mobility += popcount(moves);

                if (outpost[side] & square_bb(sq))
                {
                    no_knight_outpost++;
                }
            }

            *features++ = no_safe_knights;
            *features++ = no_knight_control_center;
            *features++ = no_knight_king_protector;
            *features++ = no_knight_mobility;
            *features++ = no_knight_outpost;
        }

        // bishop
        {
            constexpr Piece piece = make_piece(side, BISHOP);
            Bitboard opponent_pieces =
                position.pieces(!side) & ~position.pieces(!side, PAWN);
            int no_pieces = position.number_of_pieces(piece);

            int no_bishop_control_center = 0;
            int no_bishop_king_protector = 0;
            int no_bishop_mobility = 0;
            int no_bishop_outpost = 0;
            int no_bishop_pawns_on_the_same_color = 0;
            int no_bishop_pair = 0;

            no_bishop_control_center += popcount(attacked_by[side][BISHOP] & OPPONENT_RANKS[side]);
            for (int i = 0; i < no_pieces; ++i)
            {
                Square sq = position.piece_position(piece, i);
                Bitboard attacking = slider_attack<BISHOP>(sq, position.pieces());

                Bitboard moves = attacking;
                if (square_bb(sq) & blockers_for_king[side])
                {
                    moves &= FULL_LINES[sq][ownKing];
                }
                // cannot move into square with our piece
                moves &= ~position.pieces(side);
                // attacked by opponents pawns
                moves &= ~(attacked_by[!side][PAWN] & ~opponent_pieces);
                // attacked by opponents piece and not defended by our pawns
                moves &=
                    ~(attacked_by_piece[!side] & ~attacked_by[side][PAWN]);

                no_bishop_mobility += popcount(moves);
                no_bishop_king_protector += distance(ownKing, sq);
                no_bishop_pawns_on_the_same_color += popcount(position.pieces(side, PAWN) & color_squares[sq_color(sq)]);

                if (outpost[side] & square_bb(sq))
                {
                    no_bishop_outpost++;
                }
            }
            if ((position.pieces(side, BISHOP) & white_squares_bb) &&
                (position.pieces(side, BISHOP) & black_squares_bb))
                no_bishop_pair = 1;

            *features++ = no_bishop_control_center;
            *features++ = no_bishop_king_protector;
            *features++ = no_bishop_mobility;
            *features++ = no_bishop_outpost;
            *features++ = no_bishop_pawns_on_the_same_color;
            *features++ = no_bishop_pair;
        }

        // rook
        {
            constexpr Piece piece = make_piece(side, ROOK);
            Bitboard opponent_pieces =
                position.pieces(!side) & ~position.pieces(!side, PAWN);
            int no_pieces = position.number_of_pieces(piece);


            int no_rook_control = 0;
            int no_rook_semiopen_file = 0;
            int no_rook_open_file = 0;
            int no_rook_connected = 0;
            int no_rook_mobility = 0;

            no_rook_control += popcount(attacked_by[side][ROOK] & OPPONENT_RANKS[side]);
            for (int i = 0; i < no_pieces; ++i)
            {
                Square sq = position.piece_position(piece, i);

                Bitboard file_bb = FILES_BB[file(sq)];
                if (!(file_bb & position.pieces(PAWN)))
                    no_rook_open_file++;
                if (!(file_bb & position.pieces(side, PAWN)) &&
                    file_bb & position.pieces(!side, PAWN))
                    no_rook_semiopen_file++;

                if (popcount_more_than_one(file_bb & position.pieces(piece)))
                    no_rook_connected = 1;

                Bitboard attacking = slider_attack<ROOK>(sq, position.pieces());

                Bitboard moves = attacking;
                if (square_bb(sq) & blockers_for_king[side])
                {
                    moves &= FULL_LINES[sq][ownKing];
                }
                // cannot move into square with our piece
                moves &= ~position.pieces(side);
                // attacked by opponents pawns
                moves &= ~(attacked_by[!side][PAWN] & ~opponent_pieces);
                // attacked by opponents piece and not defended by our pawns
                moves &=
                    ~(attacked_by_piece[!side] & ~attacked_by[side][PAWN]);
                no_rook_mobility += popcount(moves);
            }

            *features++ = no_rook_control;
            *features++ = no_rook_semiopen_file;
            *features++ = no_rook_open_file;
            *features++ = no_rook_connected;
            *features++ = no_rook_mobility;
        }

        // queen
        {
            constexpr Piece piece = make_piece(side, QUEEN);
            Bitboard opponent_pieces =
                position.pieces(!side) & ~position.pieces(!side, PAWN);
            int no_pieces = position.number_of_pieces(piece);

            int no_queen_control = 0;
            int no_queen_weak = 0;
            int no_queen_mobility = 0;

            no_queen_control += popcount(attacked_by[side][QUEEN] & OPPONENT_RANKS[side]);
            for (int i = 0; i < no_pieces; ++i)
            {
                Square sq = position.piece_position(piece, i);

                Bitboard snipers =
                    (pseudoattacks<BISHOP>(sq) & position.pieces(!side, BISHOP)) |
                    (pseudoattacks<ROOK>(sq) & position.pieces(!side, ROOK));
                Bitboard rest = position.pieces() & ~(snipers | square_bb(sq));

                while (snipers)
                {
                    Square sniper_sq = Square(pop_lsb(&snipers));
                    Bitboard blockers = LINES[sq][sniper_sq] & rest;
                    // check if there is only one blocker between the queen and the
                    // sniper
                    if (blockers && !popcount_more_than_one(blockers))
                    {
                        no_queen_weak++;
                        break;
                    }
                }

                Bitboard attacking = slider_attack<QUEEN>(sq, position.pieces());
                Bitboard moves = attacking;
                if (square_bb(sq) & blockers_for_king[side])
                {
                    moves &= FULL_LINES[sq][ownKing];
                }
                // cannot move into square with our piece
                moves &= ~position.pieces(side);
                // attacked by opponents pawns
                moves &= ~(attacked_by[!side][PAWN] & ~opponent_pieces);
                // attacked by opponents piece and not defended by our pawns
                moves &=
                    ~(attacked_by_piece[!side] & ~attacked_by[side][PAWN]);
                no_queen_mobility += popcount(moves);
            }

            *features++ = no_queen_control;
            *features++ = no_queen_weak;
            *features++ = no_queen_mobility;
        }

        return features;
    }

    template <Color side>
    int* generate_pawns(int* features)
    {
        constexpr Piece pawn = make_piece(side, PAWN);
        constexpr Direction up_dir = side == WHITE ? NORTH : SOUTH;
        constexpr Direction down_dir = static_cast<Direction>(-up_dir);
        constexpr int up = side == WHITE ? 1 : -1;

        int num_pawns = position.number_of_pieces(pawn);

        Bitboard ourPawns = position.pieces(side, PAWN);
        Bitboard theirPawns = position.pieces(!side, PAWN);

        int no_pawn_doubled = 0;
        int no_pawn_support = 0;
        int no_pawn_isolated = 0;
        int no_pawn_backward = 0;
        int no_pawn_passed = 0;


        for (int i = 0; i < num_pawns; ++i)
        {
            Square sq = position.piece_position(pawn, i);

            Rank r = rank(sq);
            File f = file(sq);
            Rank rel_rank = relative_rank<side>(r);

            Bitboard neighbours = ourPawns & NEIGHBOUR_FILES_BB[f];
            Bitboard phalanx = neighbours & RANKS_BB[r];
            Bitboard support = neighbours & RANKS_BB[r - up];
            Bitboard lever = theirPawns & pawn_attacks<side>(square_bb(sq));
            Bitboard leverPush =
                theirPawns & shift<up_dir>(pawn_attacks<side>(square_bb(sq)));
            Bitboard opposed = theirPawns & passed_pawn_bb<side>(sq);
            bool blocked =
                static_cast<bool>(theirPawns & shift<up_dir>(square_bb(sq)));
            bool doubled =
                static_cast<bool>(ourPawns & shift<down_dir>(square_bb(sq)));
            bool backward =
                static_cast<bool>(neighbours &
                                  passed_pawn_bb<!side>(Square(sq + 8 * up))) &&
                (blocked || leverPush);

            bool passed = !opposed || !(opposed ^ lever) ||
                          (!(opposed ^ leverPush) &&
                           popcount(phalanx) >= popcount(leverPush));

            if (doubled) no_pawn_doubled++;

            if (support | phalanx)
            {
                no_pawn_support += (1 + static_cast<int>(bool(phalanx)) - static_cast<int>(bool(opposed))) + popcount(support);
            }
            else if (!neighbours)
                no_pawn_isolated++;
            else if (backward)
                no_pawn_backward++;

            if (passed)
                no_pawn_passed += static_cast<int>(rel_rank);
        }

        *features++ = no_pawn_doubled;
        *features++ = no_pawn_support;
        *features++ = no_pawn_isolated;
        *features++ = no_pawn_backward;
        *features++ = no_pawn_passed;

        return features;
    }

    template <Color side>
    int* generate_king(int* features)
    {
        const Square ownKing = position.piece_position(make_piece(side, KING), 0);
        const Square opponentsKing =
            position.piece_position(make_piece(!side, KING), 0);

        const Rank first_rank = side == WHITE ? RANK_1 : RANK_8;
        const Rank second_rank = side == WHITE ? RANK_2 : RANK_7;


        Bitboard king_area = KING_MASK[ownKing] | square_bb(ownKing);

        Bitboard pawns_in_king_area = king_area & position.pieces(side, PAWN);

        *features++ = popcount(pawns_in_king_area);

        int no_king_backrank = 0;
        int no_king_weak_rays = 0;
        int no_king_diagonals = 0;
        int no_king_lines = 0;

        if (rank(ownKing) == first_rank && position.pieces(!side, ROOK, QUEEN))
        {
            // check for backrank weakness
            Bitboard backrank_area = king_area & RANKS_BB[second_rank];
            Bitboard blocked = position.pieces(side) | attacked_by_piece[!side] |
                               attacked_by[!side][PAWN] |
                               KING_MASK[opponentsKing];
            if ((backrank_area & blocked) == backrank_area)
                no_king_backrank = 1;
        }

        no_king_weak_rays += popcount(blockers_for_king[side] & ~(position.pieces(side, PAWN)));

        Bitboard possible_bishop_check =
            slider_attack<BISHOP>(ownKing, position.pieces());
        if (position.pieces(!side, QUEEN) ||
            (position.pieces(!side, BISHOP) & color_squares[sq_color(ownKing)]))
            no_king_diagonals += popcount(possible_bishop_check);
        Bitboard possible_rook_check =
            slider_attack<ROOK>(ownKing, position.pieces());
        if (position.pieces(!side, QUEEN) || position.pieces(!side, ROOK))
            no_king_lines += popcount(possible_rook_check);

        *features++ = no_king_backrank;
        *features++ = no_king_weak_rays;
        *features++ = no_king_diagonals;
        *features++ = no_king_lines;

        return features;
    };

    template <Color side>
    void setup()
    {
        const Square ownKing = position.piece_position(make_piece(side, KING), 0);
        const Square opponentsKing =
            position.piece_position(make_piece(!side, KING), 0);

        attacked_by[side][PAWN] =
            pawn_attacks<side>(position.pieces(make_piece(side, PAWN)));

        attacked_by[side][KNIGHT] = 0ULL;
        for (int i = 0; i < position.number_of_pieces(make_piece(side, KNIGHT));
             ++i)
        {
            Square sq = position.piece_position(make_piece(side, KNIGHT), i);
            attacked_by[side][KNIGHT] |= KNIGHT_MASK[sq];
        }

        attacked_by[side][BISHOP] = 0ULL;
        for (int i = 0; i < position.number_of_pieces(make_piece(side, BISHOP));
             ++i)
        {
            Square sq = position.piece_position(make_piece(side, BISHOP), i);
            // remove own bishops and queens from blockers (x-ray attack)
            Bitboard blockers =
                position.pieces() & (~position.pieces(side, BISHOP, QUEEN));
            attacked_by[side][BISHOP] |= slider_attack<BISHOP>(sq, blockers);
        }

        attacked_by[side][ROOK] = 0ULL;
        for (int i = 0; i < position.number_of_pieces(make_piece(side, ROOK)); ++i)
        {
            Square sq = position.piece_position(make_piece(side, ROOK), i);
            // remove own rooks and queens from blockers (x-ray attack)
            Bitboard blockers =
                position.pieces() & (~position.pieces(side, ROOK, QUEEN));
            attacked_by[side][ROOK] |= slider_attack<ROOK>(sq, blockers);
        }

        attacked_by[side][QUEEN] = 0ULL;
        for (int i = 0; i < position.number_of_pieces(make_piece(side, QUEEN)); ++i)
        {
            Square sq = position.piece_position(make_piece(side, QUEEN), i);
            // remove own bishops and queens from blockers (x-ray attack)
            Bitboard blockers =
                position.pieces() & (~position.pieces(side, BISHOP, QUEEN));
            attacked_by[side][QUEEN] |= slider_attack<BISHOP>(sq, blockers);
            // remove own bishops and rooks from blockers (x-ray attack)
            blockers = position.pieces() & (~position.pieces(side, ROOK, QUEEN));
            attacked_by[side][QUEEN] |= slider_attack<ROOK>(sq, blockers);
        }

        attacked_by[side][KING] =
            KING_MASK[position.piece_position(make_piece(side, KING), 0)];

        attacked_by_piece[side] =
            attacked_by[side][KNIGHT] | attacked_by[side][BISHOP] |
            attacked_by[side][ROOK] | attacked_by[side][QUEEN];

        outpost[side] = get_outposts<side>(position);

        blockers_for_king[side] =
            blockers_for_square<side>(position, ownKing, snipers_for_king[side]);
    }


    Position position;
    Bitboard attacked_by[COLOR_NUM][PIECE_KIND_NUM];
    Bitboard attacked_by_piece[COLOR_NUM];
    Bitboard outpost[COLOR_NUM];
    Bitboard blockers_for_king[COLOR_NUM];
    Bitboard snipers_for_king[COLOR_NUM];
};

extern "C" int number_of_features()
{
    return NUM_FEATURES;
}

extern "C" void generate_features(char* fen, int* features)
{
    FeatureGenerator gen{std::string(fen)};
    gen.generate(features);
}

extern "C" void parse_position(char* fen, int* arr)
{
    Position position{std::string(fen)};

    int c = position.color() == WHITE ? 1 : 0;

    const int NUM_LAYERS = PIECE_NUM + 1;
    std::memset(arr, 0, SQUARE_NUM * NUM_LAYERS * sizeof(int));

    for (int sq = 0; sq < SQUARE_NUM; ++sq)
    {
        Piece p = position.piece_at(Square(sq));
        if (p != NO_PIECE)
            arr[sq * NUM_LAYERS + p] = 1;
        arr[sq * NUM_LAYERS + PIECE_NUM] = c;
    }
}

extern "C" void init()
{
    init_move_bitboards();
    init_zobrist();
    bitbase::init();
    endgame::init();
}
