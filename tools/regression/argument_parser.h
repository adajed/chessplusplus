#pragma once

struct TimeFormat
{
    int time_initial_ms;
    int time_increment_ms;
};

struct EngineParams
{
    std::string command;
    std::string name;
};

struct Args
{
    Args() : engines(), time_format{300000, 3000}, num_games(1), debug(false), repeat(false), polyglot(""), book_depth(0), pgn_file("games.pgn") {}

    std::vector<EngineParams> engines;
    TimeFormat time_format;
    int num_games;
    bool debug;
    bool repeat;
    std::string polyglot;
    int book_depth;
    std::string pgn_file;
};

Args parse_args(int argc, char** argv)
{
    std::vector<std::string> tokens;
    for (int i = 0; i < argc; ++i)
    {
        tokens.push_back(std::string(argv[i]));
    }

    Args args;

    int pos = 1;
    while (pos < argc)
    {
        if (tokens[pos] == "--engine" || tokens[pos] == "-e")
        {

            EngineParams params;
            pos++;
            while (pos < argc && tokens[pos].find("-") != 0)
            {
                size_t idx = tokens[pos].find("=");
                std::string cmd = tokens[pos].substr(0, idx);
                std::string value = tokens[pos].substr(idx + 1);
                if (cmd == "command")
                {
                    params.command = value;
                }
                else if (cmd == "name")
                {
                    params.name = value;
                }
                else
                {
                    std::cerr << "Unknown engine param: " << cmd << std::endl;
                }

                pos++;
            }

            args.engines.push_back(params);
        }
        else if (tokens[pos] == "--time" || tokens[pos] == "-t")
        {
            pos++;
            size_t idx = tokens[pos].find("+");
            TimeFormat time_format;
            time_format.time_initial_ms = std::stoi(tokens[pos].substr(0, idx)) * 60000;
            time_format.time_increment_ms = std::stoi(tokens[pos].substr(idx + 1)) * 1000;
            args.time_format = time_format;
            pos++;
        }
        else if (tokens[pos] == "--numgames" || tokens[pos] == "-n")
        {
            pos++;
            args.num_games = std::stoi(tokens[pos++]);
        }
        else if (tokens[pos] == "--debug")
        {
            pos++;
            args.debug = true;
        }
        else if (tokens[pos] == "--repeat")
        {
            pos++;
            args.repeat = true;
        }
        else if (tokens[pos] == "--polyglot" || tokens[pos] == "-p")
        {
            pos++;
            args.polyglot = tokens[pos++];
        }
        else if (tokens[pos] == "--bookdepth")
        {
            pos++;
            args.book_depth = std::stoi(tokens[pos++]);
        }
        else if (tokens[pos] == "--pgn")
        {
            pos++;
            args.pgn_file = tokens[pos++];
        }
        else
        {
            std::cerr << "Unknown argument: " << tokens[pos] << std::endl;
            pos++;
        }
    }

    return args;
}
