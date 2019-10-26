#include <chrono>
#include <iostream>

#include "movegen.h"
#include "score.h"
#include "search.h"
#include "weights.h"
#include "zobrist_hash.h"

using namespace engine;

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

uint64_t duration(TimePoint start, TimePoint end)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

Move parse_move(std::string move_str)
{
    if (move_str == "OO")
        return create_castling(KING_CASTLING);
    if (move_str == "OOO")
        return create_castling(QUEEN_CASTLING);

    if (move_str.size() >= 4 && move_str.size() <= 5)
    {
        int file1 = move_str[0] - 'a';
        int rank1 = move_str[1] - '1';
        int file2 = move_str[2] - 'a';
        int rank2 = move_str[3] - '1';

        Square from = make_square(Rank(rank1), File(file1));
        Square to   = make_square(Rank(rank2), File(file2));
        PieceKind promotion = NO_PIECE_KIND;

        if (move_str.size() == 5)
        {
            switch (move_str[4])
            {
                case 'N': promotion = KNIGHT; break;
                case 'B': promotion = BISHOP; break;
                case 'R': promotion = ROOK;   break;
                case 'Q': promotion = QUEEN;  break;
            }
        }

        if (promotion == NO_PIECE_KIND)
            return create_move(from, to);
        return create_promotion(from, to, promotion);
    }

    return NO_MOVE;
}

int main(int argc, char** argv)
{
    init_move_bitboards();
    zobrist::init();

    std::string weights_path(argv[1]);
    std::string fen(argv[2]);
    Position position(fen);

    Weights weights = load(weights_path);
    PositionScorer scorer(weights);
    Search search(scorer);
    search.set_thinking_time(10LL * 1000LL);

    for (int i = 3; i < argc; ++i)
    {
        std::string s(argv[i]);
        Move m = parse_move(s);

        if (!is_move_legal(position, m))
        {
            std::cout << "Illegal move" << std::endl;
            return 1;
        }
        position.do_move(m);
    }

    while (true)
    {
        std::cout << position;

        Move move = search.select_move(position, 0);

        std::cout << "move = ";
        print_move(std::cout, move);
        std::cout << std::endl;

        position.do_move(move);

        std::cout << position;

        std::string move_str;
        do
        {
            std::cin >> move_str;
            move = parse_move(move_str);
        } while (!is_move_legal(position, move));

        position.do_move(move);

    }
    std::cout << position;

    return 0;
}
