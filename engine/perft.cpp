#include <chrono>
#include <iostream>
#include <vector>

#include "movegen.h"
#include "minimax.h"

using namespace engine;

uint32_t perft(Position& position, int depth)
{
    if (depth == 0)
        return 0;

    Move* begin = MOVE_LIST[depth];
    Move* end = generate_moves(position, begin);

    if (depth == 1)
        return end - begin;


    uint32_t sum = 0;
    for (Move* it = begin; it != end; ++it)
    {
        do_move(position, *it);
        sum += perft(position, depth - 1);
        undo_move(position, *it);
    }

    return sum;
}

std::vector<std::pair<Move, uint32_t>> move_perft(Position& position, int depth)
{
    Move* begin = MOVE_LIST[depth];
    Move* end = generate_moves(position, begin);

    std::vector<std::pair<Move, uint32_t>> scores(end - begin);

    uint32_t sum = 0;
    for (Move* it = begin; it != end; ++it)
    {
        int i = it - begin;
        scores[i].first = *it;
        do_move(position, *it);
        scores[i].second = perft(position, depth - 1);
        undo_move(position, *it);
    }

    return scores;
}

int main()
{
    init_move_bitboards();

    std::vector<std::string> fens = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ",
    };

    for (auto& fen : fens)
    {
        Position position(fen);
        std::cout << position;

        for (int depth = 1; depth < 7; ++depth)
        {
            position = Position(fen);
            auto start = std::chrono::steady_clock::now();
            uint32_t score = perft(position, depth);
            auto end = std::chrono::steady_clock::now();

            auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            double time_d = double(time) * 1e-6;
            uint64_t speed = uint64_t(double(score) / time_d);

            std::cout << "[depth " << depth << "] score=" << score << ", time=" << time_d << "s, speed=" << speed << " nodes/s" << std::endl;
        }

        std::cout << std::endl;
    }
    return 0;
}
