#include <chrono>
#include <iostream>

#include "score.h"
#include "search.h"
#include "weights.h"
#include "zobrist_hash.h"

using namespace engine;

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

const unsigned long long MOVE_TIME = 120ULL * 1000000ULL;

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
    init_zobrist_hash();

    std::string weights_path(argv[1]);
    std::string fen(argv[2]);
    Position position = from_fen(fen);

    Weights weights(weights_path);
    PositionScorer scorer(weights);
    Search search(scorer);
    search.set_thinking_time(120ULL * 1000000ULL);

    for (int i = 2; i < argc; ++i)
    {
        std::string s(argv[i]);
        Move m = parse_move(s);
        do_move(position, m);
    }

    while (true)
    {
        std::cout << position;

        Move move = search.select_move(position, 0);

        std::cout << "move = ";
        print_move(std::cout, move);
        std::cout << std::endl;

        do_move(position, move);

        std::cout << position;

        std::string move_str;
        std::cin >> move_str;
        move = parse_move(move_str);
        do_move(position, move);

    }
    std::cout << position;

    return 0;
}
