#include "eco_codes.h"

#include "extended_position.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>

namespace eco
{

std::vector<std::string> split(std::string str, std::string delim)
{
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delim)) != std::string::npos)
    {
        token = str.substr(0, pos);
        tokens.push_back(token);
        str.erase(0, pos + delim.length());
    }
    tokens.push_back(str);
    return tokens;
}

std::vector<Code> ECO_CODES;

void init()
{
    const std::regex ecoRegex{"([ABCDE][[:digit:]]{2}(?:(?:)|[a-z]|(?:[a-z][[:digit:]])))[[:space:]]*\"(.*)\"[[:space:]]*(.*)$"};
    const std::regex sanRegex{"(?:[[:digit:]]+[.])?((?:[NBRQK]?[a-h]?[1-8]?x?[a-h][1-8]=?[nbrqkNBRQK]?[\\+#]?)|(?:[0O](?:-[0O]){1,2}))"};

    std::string line;
    std::ifstream fd{ECO_CODES_FILE};

    Code currentCode;

    std::smatch match;
    engine::Position position;

    while (std::getline(fd, line))
    {
        if (line.starts_with("#"))
            continue;

        if (std::regex_match(line, match, ecoRegex))
        {
            if (currentCode.code != "")
            {
                currentCode.fen = position.fen();
                ECO_CODES.push_back(currentCode);
            }

            currentCode = Code{{}, match[1], match[2], ""};
            position = engine::Position{};
            line = match[3];
        }

        for (std::string san : split(line, " "))
        {
            std::smatch m;
            if (std::regex_match(san, m, sanRegex))
            {
                engine::Move move = position.parse_san(m[1]);
                assert(move != engine::NO_MOVE);
                position.do_move(move);
                currentCode.moveList.push_back(move);
            }
        }
    }

    if (currentCode.code != "")
    {
        currentCode.fen = position.fen();
        ECO_CODES.push_back(currentCode);
    }

    fd.close();
}

void printAllCodes()
{
    std::cout << "size = " << ECO_CODES.size() << std::endl;
    std::for_each(ECO_CODES.begin(), ECO_CODES.end(),
            [](const Code& eco)
            {
            engine::Position position;
            std::cout << eco.code << " \"" << eco.openingName << "\" Moves: ";
            std::for_each(eco.moveList.begin(), eco.moveList.end(),
                    [&position](engine::Move move)
                    {
                    std::cout << position.san(move) << " ";
                    position.do_move(move);
                    });
            std::cout << std::endl;
            });
}

Code get(const ExtendedPosition &position)
{
    auto commonPrefixLength = [&position](const Code& eco)
    {
        for (uint32_t i = 0; i < eco.moveList.size(); ++i)
        {
            if (i >= position.size() || eco.moveList[i] != position[i].move)
                return 0ul;
        }
        return eco.moveList.size();
    };

    return *std::max_element(ECO_CODES.begin(), ECO_CODES.end(),
            [&commonPrefixLength](const auto& eco1, const auto& eco2)
            {
            return commonPrefixLength(eco1) < commonPrefixLength(eco2);
            });
}

}  // namespace eco
