#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "position.h"
#include "score.h"
#include "endgame.h"
#include "movegen.h"
#include "zobrist_hash.h"

using namespace engine;

std::vector<std::string> split_by(std::string s, char sep = ',')
{
    std::vector<std::string> vec;

    size_t pos = 0;
    do
    {
        size_t next_pos = s.find_first_of(sep, pos);
        size_t len = next_pos == std::string::npos ? std::string::npos : (next_pos - pos);
        vec.push_back(s.substr(pos, len));
        pos = next_pos == std::string::npos ? std::string::npos : (next_pos + 1);
    } while (pos != std::string::npos);

    return vec;
}

int get_position(const std::vector<std::string>& vec, const std::string& s)
{
    for (int i = 0; i < (int) vec.size(); ++i)
    {
        if (vec[i] == s) return i;
    }

    return -1;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " filepath" << std::endl;
        return 1;
    }

    init_move_bitboards();
    init_zobrist();
    bitbase::init();
    endgame::init();

    std::string filepath(argv[1]);

    std::ifstream fd(filepath);

    std::string header;
    std::getline(fd, header);
    auto headers = split_by(header);
    for (auto const& s : headers)
        std::cout << s << std::endl;

    int fen_position = get_position(headers, "fen");
    if (fen_position == -1)
    {
        std::cout << "No \"fen\" fields in file" << std::endl;
        return 1;
    }
    int result_position = get_position(headers, "result");
    if (result_position == -1)
    {
        std::cout << "No \"result\" fields in file" << std::endl;
        return 1;
    }

    PositionScorer scorer;
    size_t no_correct = 0;
    size_t no_all = 0;

    std::string line;
    while (!fd.eof())
    {
        std::getline(fd, line);
        auto values = split_by(line);

        if (headers.size() != values.size())
        {
            std::cout << "Wrong number of elements, omitting" << std::endl;
            continue;
        }

        std::string fen = values[fen_position];
        int result = std::stoi(values[result_position]);

        no_all++;

        Position position(fen);
        Value value = scorer.score(position);
        value = position.color() == WHITE ? value : -value;

        if ((result > 0 && value > 0) || (result < 0 && value < 0))
            no_correct++;
    }

    std::cout << "Accuracy = " << no_correct << "/" << no_all << " " << static_cast<double>(no_correct) / static_cast<double>(no_all) << std::endl;

    return 0;
}
