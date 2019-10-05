#include <chrono>
#include <iostream>

#include "engine.h"

using namespace engine;

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

const unsigned long long MOVE_TIME = 30ULL * 1000000ULL;

double duration(TimePoint start, TimePoint end)
{
    uint64_t t = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return double(t) * 10e-6;
}

Move parse_move(std::string move_str)
{
    Move move;
    bool correct = false;

    while (!correct)
    {
        if (move_str == "OO")
        {
            move = Move{NO_SQUARE, NO_SQUARE, NO_PIECE_KIND, NO_PIECE_KIND, KING_CASTLING, NO_CASTLING, NO_SQUARE, false};
            correct = true;
        }
        else if (move_str == "OOO")
        {
            move = Move{NO_SQUARE, NO_SQUARE, NO_PIECE_KIND, NO_PIECE_KIND, QUEEN_CASTLING, NO_CASTLING, NO_SQUARE, false};
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
                PieceKind piece_kind = NO_PIECE_KIND;

                if (move_str.size() == 5)
                {
                    switch (move_str[4])
                    {
                        case 'N': piece_kind = KNIGHT; correct = true; break;
                        case 'B': piece_kind = BISHOP; correct = true; break;
                        case 'R': piece_kind = ROOK; correct = true; break;
                        case 'Q': piece_kind = QUEEN; correct = true; break;
                    }
                }
                else
                {
                    correct = true;
                }

                move = Move{from, to, NO_PIECE_KIND, piece_kind, NO_CASTLING, NO_CASTLING, NO_SQUARE, false};
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

        auto start_time = std::chrono::steady_clock::now();
        auto end_time = start_time;

        int depth = 0;
        while (duration(start_time, end_time) < 20.)
        {
            depth += 1;
            scored_move = minimax(position, depth, true);
            end_time = std::chrono::steady_clock::now();

            double time = duration(start_time, end_time);
            std::cout << "depth=" << depth
                      << ", score=" << scored_move.score
                      << ", time=" << time << "s"
                      << std::endl;

            if (scored_move.score == (1ULL << 16) ||
                    scored_move.score == -(1ULL << 16))
                break;
        }

        double time = duration(start_time, end_time);
        std::cout << "move " << scored_move.move << std::endl;
        std::cout << "score " << scored_move.score << std::endl;
        std::cout << "time " << time << "s" << std::endl;

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
