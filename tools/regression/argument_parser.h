#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

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
    Args()
        : engines(),
          time_format{300000, 3000},
          num_games(1),
          debug(false),
          repeat(false),
          polyglot(""),
          book_depth(0),
          pgn_file("games.pgn"),
          num_threads(1),
          seed(std::chrono::system_clock::now().time_since_epoch().count())
    {
    }

    std::vector<EngineParams> engines;
    TimeFormat time_format;
    int num_games;
    bool debug;
    bool repeat;
    std::string polyglot;
    int book_depth;
    std::string pgn_file;
    int num_threads;
    size_t seed;
};

Args parse_args(int argc, char** argv)
{
    std::vector<std::string> tokens;
    for (int i = 0; i < argc; ++i)
    {
        tokens.push_back(std::string(argv[i]));
    }

    if (std::find(tokens.begin(), tokens.end(), "--help") != tokens.end())
    {
        std::cout << "Usage: " << tokens[0] << " [option [option ...]]" << std::endl << std::endl;

        std::cout << "Options:" << std::endl;
        std::cout << "\t--engine [engineOption [engineOption ...]] - Adds new engine with given parameters to regression" << std::endl;
        std::cout << "\t--time - Time format: <intial_time in minutes>:<increment in seconds> (default: 5+3)" << std::endl;
        std::cout << "\t--numgames - Number of games to run (default: 1)" << std::endl;
        std::cout << "\t--threads - Number of threads to use (default: 1)" << std::endl;
        std::cout << "\t--pgn - Path to PGN files where games will be saved (default: games.pgn)" << std::endl;
        std::cout << "\t--polyglot - Path to polyglot book used for openings (default: not used)" << std::endl;
        std::cout << "\t--bookdepth - Depth (in plies) up to which the book is used (default: 0)" << std::endl;
        std::cout << "\t--repeat - Repeat opening for both sides" << std::endl;
        std::cout << "\t--debug - Print debug messages" << std::endl;
        std::cout << std::endl;

        std::cout << "Engine options:" << std::endl;
        std::cout << "\tcommand=<command> - command used to run engine" << std::endl;
        std::cout << "\tname=<name> - name of the engine" << std::endl;
        std::cout << std::endl;
        exit(0);
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
            time_format.time_initial_ms =
                std::stoi(tokens[pos].substr(0, idx)) * 60000;
            time_format.time_increment_ms =
                std::stoi(tokens[pos].substr(idx + 1)) * 1000;
            args.time_format = time_format;
            pos++;
        }
        else if (tokens[pos] == "--numgames" || tokens[pos] == "-n")
        {
            pos++;
            args.num_games = std::stoi(tokens[pos++]);
        }
        else if (tokens[pos] == "--threads")
        {
            pos++;
            args.num_threads = std::stoi(tokens[pos++]);
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
        else if (tokens[pos] == "--seed")
        {
            pos++;
            args.seed = std::stoll(tokens[pos++]);
        }
        else
        {
            std::cerr << "Unknown argument: " << tokens[pos] << std::endl;
            pos++;
        }
    }

    return args;
}
