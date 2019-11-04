#include "uci.h"
#include "logger.h"

#include <thread>

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
    logger << "Chess engine by Adam Jedrych"
           << " (build " << __DATE__ << " " << __TIME__ << ")" << std::endl;

    position = Position();
    quit = false;

    std::string line;
    std::string token;

    while (!quit && std::getline(std::cin, line))
    {
        logger.fout << line << std::endl;
        std::istringstream istream(line);
        istream >> token;

        bool b = false;

#define COMMAND(name)                       \
        if (token == #name)                 \
        {                                   \
            b = name##_command(istream);    \
        }                                   \

        COMMAND(uci)
        COMMAND(ucinewgame)
        COMMAND(isready)
        COMMAND(setoption)
        COMMAND(position)
        COMMAND(go)
        COMMAND(stop)
        COMMAND(ponderhit)
        COMMAND(quit)
        COMMAND(printboard)
        COMMAND(hash)
        COMMAND(perft)

#undef COMMAND

        if (!b)
        {
            logger << "Unknown command" << std::endl;
        }
    }
}

bool Uci::uci_command(std::istringstream& istream)
{
    logger << "id name Deep Chess" << std::endl;
    logger << "id author Adam Jedrych" << std::endl;
    logger << std::endl;

    logger << "uciok" << std::endl;
    return true;
}

bool Uci::ucinewgame_command(std::istringstream& istream)
{
    position = Position();
    return true;
}

bool Uci::isready_command(std::istringstream& istream)
{
    logger << "readyok" << std::endl;
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

        position.do_move(position.parse_move(token));
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
                limits.searchmoves[limits.searchmovesnum++] = position.parse_move(token);
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
    logger << position << std::endl;
    return true;
}

bool Uci::hash_command(std::istringstream& istream)
{
    logger << "Hex: " << std::hex << position.hash() << std::dec << std::endl;
    return true;
}

bool Uci::perft_command(std::istringstream& istream)
{
    int depth;
    istream >> depth;

    uint64_t sum = 0;
    if (depth > 0)
    {
        Move* begin = MOVE_LIST[depth];
        Move* end = generate_moves(position, position.side_to_move(), begin);

        for (Move* it = begin; it != end; ++it)
        {
            Move move = *it;
            MoveInfo moveinfo = position.do_move(move);

            uint64_t n = perft(position, depth - 1);

            position.undo_move(move, moveinfo);

            logger << position.move_to_string(move) << ": " << n << std::endl;
            sum += n;
        }
    }

    logger << std::endl;
    logger << "Number of nodes: " << sum << std::endl;

    return true;
}

}
