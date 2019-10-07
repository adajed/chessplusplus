#include "minimax.h"

#include <vector>
#include <thread>
#include <future>

namespace engine
{

const int64_t MIN_VALUE = -(1ULL << 16);
const int64_t MAX_VALUE = 1ULL << 16;

const int64_t INIT_ALPHA = -(1ULL << 32);
const int64_t INIT_BETA = 1ULL << 32;

const int64_t PAWN_POSITION_VALUE[] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    10, 10, 10,  0,  0, 10, 10, 10,
    5, 5, 5, 40, 40, 5, 5, 5,
    5, 5, 5, 40, 40, 5, 5, 5,
    15, 25, 30, 40, 40, 30, 25, 15,
    20, 30, 40, 50, 50, 40, 30, 20,
    30, 40, 50, 60, 60, 50, 40, 30,
    0,  0,  0,  0,  0,  0,  0,  0,
};

const int64_t KNIGHT_POSITION_VALUE[] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,
    10, 20, 20, 20, 20, 20, 20, 10,
    20, 30, 40, 40, 40, 40, 30, 20,
    20, 30, 40, 40, 40, 40, 30, 20,
    30, 40, 60, 60, 60, 60, 40, 30,
    30, 40, 70, 60, 60, 70, 40, 30,
    20, 30, 40, 40, 40, 40, 30, 20,

};

const int64_t BISHOP_POSITION_VALUE[] = {
    40, 30, 20, 10, 10, 20, 30, 40,
    40, 40, 30, 50, 50, 30, 40, 40,
    40, 40, 50, 50, 50, 50, 40, 40,
    40, 50, 60, 70, 70, 60, 50, 40,
    30, 40, 50, 60, 60, 50, 40, 30,
    30, 50, 60, 60, 60, 60, 50, 30,
    30, 50, 60, 60, 60, 60, 50, 30,
    30, 40, 50, 60, 60, 50, 40, 30,
};

const int64_t ROOK_POSITION_VALUE[] = {
     40,  40,  40,  40,  40,  40,  40,  40,
     40,  40,  40,  40,  40,  40,  40,  40,
     40,  40,  40,  40,  40,  40,  40,  40,
     40,  40,  40,  40,  40,  40,  40,  40,
    100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100,
};

const int64_t QUEEN_POSITION_VALUE[] = {
    80, 70,  60,  50,  50,  60,  70,  80,
    80, 80,  70,  90,  90,  70,  80,  80,
    80, 80,  90,  90,  90,  90,  80,  80,
    80, 90, 100, 110, 110, 100,  90,  80,
    130, 140, 150, 160, 160, 150, 140, 130,
    130, 150, 160, 160, 160, 160, 150, 130,
    130, 150, 160, 160, 160, 160, 150, 130,
    130, 140, 150, 160, 160, 150, 140, 130,
};

const int64_t KING_POSITION_VALUE[] = {
     50,  60,  40, -10, -10,  30,  60,  50,
     30,  30,   0, -30, -30,   0,  30,  30,
    -30, -30, -50, -50, -50, -50, -30, -30,
    -70, -70, -70, -70, -70, -70, -70, -70,
    -70, -70, -70, -70, -70, -70, -70, -70,
    -30, -30, -50, -50, -50, -50, -30, -30,
     30,  30,   0, -30, -30,   0,  30,  30,
     50,  60,  40, -10, -10,  30,  60,  50,
};

const int64_t QUEEN_VALUE  = 900;
const int64_t ROOK_VALUE   = 500;
const int64_t BISHOP_VALUE = 300;
const int64_t KNIGHT_VALUE = 300;
const int64_t PAWN_VALUE   = 100;

template <Color side>
Bitboard single_pawn_attack(const Position& pos)
{
    const Direction UPRIGHT = side == WHITE ? NORTHEAST : SOUTHWEST;
    const Direction UPLEFT  = side == WHITE ? NORTHWEST : SOUTHEAST;

    Bitboard pawns = pieces_bb(pos, side, PAWN);

    return shift<UPRIGHT>(pawns) | shift<UPLEFT>(pawns);
}

template <Color side>
Bitboard double_pawn_attack(const Position& pos)
{
    const Direction UPRIGHT = side == WHITE ? NORTHEAST : SOUTHWEST;
    const Direction UPLEFT  = side == WHITE ? NORTHWEST : SOUTHEAST;

    Bitboard pawns = pieces_bb(pos, side, PAWN);

    return shift<UPRIGHT>(pawns) & shift<UPLEFT>(pawns);
}

Square mirror(Square sq)
{
    return make_square(RANK_8 - rank(sq), file(sq));
}

template <Color side>
int64_t score_side(const Position& pos, int tid)
{
    const Piece PAWN_ = make_piece(side, PAWN);
    const Piece KNIGHT_ = make_piece(side, KNIGHT);
    const Piece BISHOP_ = make_piece(side, BISHOP);
    const Piece ROOK_ = make_piece(side, ROOK);
    const Piece QUEEN_ = make_piece(side, QUEEN);
    const Piece KING_  = make_piece(side, KING);

    int64_t value = 0;
    value += QUEEN_VALUE * pos.piece_count[QUEEN_];
    value += ROOK_VALUE * pos.piece_count[ROOK_];
    value += BISHOP_VALUE * pos.piece_count[BISHOP_];
    value += KNIGHT_VALUE * pos.piece_count[KNIGHT_];
    value += PAWN_VALUE * pos.piece_count[PAWN_];

    for (int i = 0; i < pos.piece_count[PAWN_]; ++i)
    {
        Square sq = pos.piece_position[PAWN_][i];
        assert(sq != NO_SQUARE);
        value += PAWN_POSITION_VALUE[side == BLACK ? mirror(sq) : sq];
    }

    for (int i = 0; i < pos.piece_count[KNIGHT_]; ++i)
    {
        Square sq = pos.piece_position[KNIGHT_][i];
        assert(sq != NO_SQUARE);
        value += KNIGHT_POSITION_VALUE[side == BLACK ? mirror(sq) : sq];
    }

    for (int i = 0; i < pos.piece_count[BISHOP_]; ++i)
    {
        Square sq = pos.piece_position[BISHOP_][i];
        assert(sq != NO_SQUARE);
        value += BISHOP_POSITION_VALUE[side == BLACK ? mirror(sq) : sq];
    }

    for (int i = 0; i < pos.piece_count[ROOK_]; ++i)
    {
        Square sq = pos.piece_position[ROOK_][i];
        assert(sq != NO_SQUARE);
        value += ROOK_POSITION_VALUE[side == BLACK ? mirror(sq) : sq];
    }

    for (int i = 0; i < pos.piece_count[QUEEN_]; ++i)
    {
        Square sq = pos.piece_position[QUEEN_][i];
        assert(sq != NO_SQUARE);
        value += QUEEN_POSITION_VALUE[side == BLACK ? mirror(sq) : sq];
    }

    for (int i = 0; i < pos.piece_count[KING_]; ++i)
    {
        Square sq = pos.piece_position[KING_][i];
        assert(sq != NO_SQUARE);
        value += KING_POSITION_VALUE[side == BLACK ? mirror(sq) : sq];
    }


    // bonus for pawns covering multiple squares
    Bitboard single_attack = single_pawn_attack<side>(pos);
    value += 10 * popcount(single_attack);
    Bitboard double_attack = single_pawn_attack<side>(pos);
    value += 20 * popcount(double_attack);

    return value;
}

int64_t score(const Position& pos, int tid)
{
    return score_side<WHITE>(pos, tid) - score_side<BLACK>(pos, tid);
}

namespace
{

std::atomic<int> counter;

ScoredMove run_minimax_inner(Position& position, int tid, int64_t alpha, int64_t beta, int depth)
{
    Color side = position.current_side;

    if (depth == 0)
        return {{}, score(position, tid)};

    Move* begin = MOVE_LIST[tid][depth];
    Move* end = generate_moves(position, tid, begin);

    if (begin == end)
    {
        if (!is_in_check(position))
            return {{}, 0};
        return {{}, side == WHITE ? MIN_VALUE : MAX_VALUE};
    }

    Move best_move = NO_MOVE;
    int64_t best_value;

    if (side == WHITE)
    {
        best_value = 2LL * MIN_VALUE;
        for (Move* it = begin; it != end; ++it)
        {
            Move move = *it;
            MoveInfo moveinfo = do_move(position, move);
            int64_t value = run_minimax_inner(position, tid, alpha, beta, depth - 1).score;
            if (value > best_value)
            {
                best_value = value;
                best_move = move;
            }
            undo_move(position, move, moveinfo);

            alpha = alpha > value ? alpha : value;
            if (alpha >= beta)
                break;
        }
    }
    else
    {
        best_value = 2LL * MAX_VALUE;
        for (Move* it = begin; it != end; ++it)
        {
            Move move = *it;
            MoveInfo moveinfo = do_move(position, move);
            int64_t value = run_minimax_inner(position, tid, alpha, beta, depth - 1).score;
            if (value < best_value)
            {
                best_value = value;
                best_move = move;
            }
            undo_move(position, move, moveinfo);

            beta = beta < value ? beta : value;
            if (alpha >= beta)
                break;
        }
    }

    return {best_move, best_value};

}

void run_minimax(std::promise<ScoredMove> && result, Position position, int tid, int64_t alpha, int64_t beta, int depth)
{
    Color side = position.current_side;

    if (depth == 0)
    {
        result.set_value({{}, score(position, tid)});
        return;
    }

    Move* begin = MOVE_LIST[tid][depth];
    Move* end = generate_moves(position, tid, begin);


    if (begin == end)
    {
        if (!is_in_check(position))
            result.set_value({{}, 0});
        else
            result.set_value({{}, side == WHITE ? MIN_VALUE : MAX_VALUE});
        return;
    }

    Move best_move = NO_MOVE;
    int64_t best_value;

    if (side == WHITE)
    {
        best_value = 2LL * MIN_VALUE;
        int i = counter++;
        while (i < end - begin)
        {
            Move move = begin[i];
            MoveInfo moveinfo = do_move(position, move);
            int64_t value = run_minimax_inner(position, tid, alpha, beta, depth - 1).score;
            if (value > best_value)
            {
                best_value = value;
                best_move = move;
            }
            undo_move(position, move, moveinfo);

            alpha = alpha > value ? alpha : value;
            if (alpha >= beta)
                break;

            i = counter++;
        }
    }
    else
    {
        best_value = 2LL * MAX_VALUE;
        int i = counter++;
        while (i < end - begin)
        {
            Move move = begin[i];
            MoveInfo moveinfo = do_move(position, move);
            int64_t value = run_minimax_inner(position, tid, alpha, beta, depth - 1).score;
            if (value < best_value)
            {
                best_value = value;
                best_move = move;
            }
            undo_move(position, move, moveinfo);

            beta = beta < value ? beta : value;
            if (alpha >= beta)
                break;

            i = counter++;
        }
    }

    result.set_value({best_move, best_value});
}

}

ScoredMove minimax(const Position& position, int depth)
{
    std::vector<std::promise<ScoredMove>> results(NUM_THREADS);
    std::vector<std::future<ScoredMove>> futures;
    std::vector<std::thread> threads;

    counter = 0;

    for (int tid = 0; tid < NUM_THREADS; ++tid)
    {
        std::promise<ScoredMove> & result = results[tid];
        futures.push_back(result.get_future());

        Position pos = position;

        threads.push_back(std::thread(
                    &run_minimax, std::move(result), pos, tid, INIT_ALPHA, INIT_BETA, depth));
    }

    for (int tid = 0; tid < NUM_THREADS; ++tid)
        threads[tid].join();

    ScoredMove best_move = futures[0].get();
    for (int tid = 1; tid < NUM_THREADS; ++tid)
    {
        ScoredMove move = futures[tid].get();

        if (move.move == NO_MOVE)
            continue;

        if (position.current_side == WHITE)
        {
            if (move.score > best_move.score)
                best_move = move;
        }
        else
        {
            if (move.score < best_move.score)
                best_move = move;
        }
    }
    return best_move;
}

uint64_t perft(Position& position, int depth, bool print_moves)
{
    if (depth == 0)
        return 1;

    Move* begin = MOVE_LIST[0][depth];
    Move* end = generate_moves(position, 0, begin);

    if (depth == 1)
    {
        if (print_moves)
            for (Move* it = begin; it != end; ++it)
            {
                print_move(*it);
                std::cout << " 1" << std::endl;
            }
        return end - begin;
    }

    uint64_t sum = 0;
    for (Move* it = begin; it != end; ++it)
    {
        Move move = *it;
        MoveInfo moveinfo = do_move(position, move);

        uint64_t nodes = perft(position, depth - 1, false);
        sum += nodes;

        if (print_moves)
        {
            print_move(move);
            std::cout << " " << nodes << std::endl;
        }

        undo_move(position, move, moveinfo);
    }

    return sum;
}

}  // namespace engine
