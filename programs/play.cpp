#include <chrono>
#include <iostream>

#include "engine.h"

using namespace engine;

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

const unsigned long long MOVE_TIME = 60ULL * 1000000ULL;

uint64_t duration(TimePoint start, TimePoint end)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

Move parse_move(std::string move_str)
{
    Move move;
    bool correct = false;

    while (!correct)
    {
        if (move_str == "OO")
        {
            move = create_castling(KING_CASTLING);
            correct = true;
        }
        else if (move_str == "OOO")
        {
            move = create_castling(QUEEN_CASTLING);
            correct = true;
        }
        else
        {
            if (move_str.size() >= 4 && move_str.size() <= 5)
            {
                int f1 = move_str[0] - 'a';
                int r1 = move_str[1] - '1';
                int f2 = move_str[2] - 'a';
                int r2 = move_str[3] - '1';

                Square from = Square(8 * r1 + f1);
                Square to   = Square(8 * r2 + f2);
                PieceKind promotion = NO_PIECE_KIND;

                if (move_str.size() == 5)
                {
                    switch (move_str[4])
                    {
                        case 'N': promotion = KNIGHT; correct = true; break;
                        case 'B': promotion = BISHOP; correct = true; break;
                        case 'R': promotion = ROOK; correct = true; break;
                        case 'Q': promotion = QUEEN; correct = true; break;
                    }
                }
                else
                {
                    correct = true;
                }

                if (promotion == NO_PIECE_KIND)
                    move = create_move(from, to);
                move = create_promotion(from, to, promotion);
            }
        }
    }

    return move;
}

int main(int argc, char** argv)
{
    init_move_bitboards();

    std::string fen(argv[1]);
    Position position = from_fen(fen);

    for (int i = 2; i < argc; ++i)
    {
        std::string s(argv[i]);
        Move m = parse_move(s);
        do_move(position, m);
    }

    Move move;
    ScoredMove scored_move;
    while (true)
    {
        std::cout << position;

        int depth = 8;
        TimePoint start_time = std::chrono::steady_clock::now();
        scored_move = minimax(position, depth);
        TimePoint end_time = std::chrono::steady_clock::now();

        uint64_t time = duration(start_time, end_time);
        std::cout << "depth = " << depth << std::endl;
        std::cout << "score = " << scored_move.score << std::endl;
        std::cout << "time = " << time << " microseconds" << std::endl;
        std::cout << "move = " << scored_move.move << std::endl;
        std::cout << std::endl;

        do_move(position, scored_move.move);

        std::cout << position;

        std::string move_str;
        std::cin >> move_str;
        move = parse_move(move_str);
        do_move(position, move);

    }
    std::cout << position;

    return 0;
}
