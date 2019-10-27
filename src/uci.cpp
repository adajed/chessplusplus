#include "uci.h"

#include <thread>

#include <iostream>

namespace engine
{

Uci::Uci(const PositionScorer& scorer)
    : scorer(scorer)
    , search(nullptr)
    , position()
    , quit(false)
{
}

void Uci::loop()
{
    std::cout << "Chess engine by Adam Jedrych"
              << " (build " << __DATE__ << " " << __TIME__ << ")" << std::endl;

    position = Position();
    quit = false;

    std::string line;
    std::string token;

    while (!quit && std::getline(std::cin, line))
    {
        std::istringstream istream(line);
        istream >> token;

        bool b = false;

        if (token == "uci")
        {
            b = uci_command(istream);
        }
        else if (token == "ucinewgame")
        {
            b = ucinewgame_command(istream);
        }
        else if (token == "isready")
        {
            b = isready_command(istream);
        }
        else if (token == "setoption")
        {
            b = setoption_command(istream);
        }
        else if (token == "position")
        {
            b = position_command(istream);
        }
        else if (token == "go")
        {
            b = go_command(istream);
        }
        else if (token == "stop")
        {
            b = stop_command(istream);
        }
        else if (token == "ponderhit")
        {
            b = ponderhit_command(istream);
        }
        else if (token == "quit")
        {
            b = quit_command(istream);
        }
        else if (token == "printboard")
        {
            b = printboard_command(istream);
        }

        if (!b)
        {
            std::cout << "Unknown command" << std::endl;
        }
    }
}

bool Uci::uci_command(std::istringstream& istream)
{
    std::cout << "id name Deep Chess" << std::endl;
    std::cout << "id author Adam Jedrych" << std::endl;
    std::cout << std::endl;

    std::cout << "uciok" << std::endl;
    return true;
}

bool Uci::ucinewgame_command(std::istringstream& istream)
{
    position = Position();
    return true;
}

bool Uci::isready_command(std::istringstream& istream)
{
    std::cout << "readyok" << std::endl;
    return true;
}

bool Uci::setoption_command(std::istringstream& istream)
{
    return true;
}

bool Uci::position_command(std::istringstream& istream)
{
    std::string token;
    istream >> token;

    if (token == "startpos")
    {
        position = Position();
    }
    else if (token == "fen")
    {
        std::string fen = "";
        while (istream >> token && token != "moves")
            fen += token + " ";

        position = Position(fen);
    }
    else
        return false;

    while (istream >> token)
    {
        if (token == "moves")
            continue;

        Move move = string_to_move(token);
        position.do_move(move);
    }

    return true;
}

bool Uci::go_command(std::istringstream& istream)
{
    Limits limits;
    std::string token;

    while (istream >> token)
    {
        if (token == "ponder")
            limits.ponder = true;
        else if (token == "wtime")
            istream >> limits.timeleft[WHITE];
        else if (token == "btime")
            istream >> limits.timeleft[BLACK];
        else if (token == "winc")
            istream >> limits.timeinc[WHITE];
        else if (token == "binc")
            istream >> limits.timeinc[BLACK];
        else if (token == "movestogo")
            istream >> limits.movestogo;
        else if (token == "depth")
            istream >> limits.depth;
        else if (token == "nodes")
            istream >> limits.nodes;
        else if (token == "mate")
            istream >> limits.mate;
        else if (token == "movetime")
            istream >> limits.movetime;
        else if (token == "infinite")
            limits.infinite = true;
        else if (token == "searchmoves")
        {
            while (istream >> token)
                limits.searchmoves[limits.searchmovesnum++] = string_to_move(token);
        }
    }

    search = std::make_shared<Search>(position, scorer, limits);

    std::thread search_thread([this](){ this->search->go(); });
    search_thread.detach();

    return true;
}

bool Uci::stop_command(std::istringstream& istream)
{
    if (search)
        search->stop();
    return true;
}

bool Uci::ponderhit_command(std::istringstream& istream)
{
    return true;
}

bool Uci::quit_command(std::istringstream& istream)
{
    if (search)
        search->stop();
    quit = true;
    return true;
}

bool Uci::printboard_command(std::istringstream& istream)
{
    std::cout << position << std::endl;
    position.threefold_repetition();
    return true;
}

}
