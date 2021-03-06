#include "uci.h"

#include "logger.h"
#include "transposition_table.h"

#include <bits/stdint-uintn.h>
#include <thread>

namespace engine
{
Uci::Uci() : search(nullptr), position(), quit(false), options(), polyglot()
{
    options["Polyglot Book"] = UciOption("", [this](std::string path) {
        if (path == "")
            this->polyglot = PolyglotBook();
        else
            this->polyglot = PolyglotBook(path);
    });
    options["Logfile"] = UciOption("", [](std::string path) {
        /* if (path == "") */
        /*     logger.close_file(); */
        /* else */
        /*     logger.open_file(path); */
    });
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
        /* logger.fout << line << std::endl; */
        std::istringstream istream(line);
        istream >> token;

        bool b = false;

#define COMMAND(name)                \
    if (token == #name)              \
    {                                \
        b = name##_command(istream); \
    }                                \
    else

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
        COMMAND(moves) { std::cout << "Unknown command" << std::endl; }

#undef COMMAND

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

    for (auto option_pair : options)
    {
        std::string name = option_pair.first;
        UciOption option = option_pair.second;
        OptionType optiontype = option.get_type();

        std::cout << "option ";
        std::cout << "name " << name << " ";
        std::cout << "type " << optiontype_to_string(optiontype) << " ";
        if (optiontype == kCHECK)
            std::cout << "default " << (option.get_check() ? "true" : "false")
                      << " ";
        else if (optiontype == kSPIN)
        {
            std::cout << "default " << option.get_spin_initial() << " ";
            std::cout << "min " << option.get_spin_min() << " ";
            std::cout << "max " << option.get_spin_max() << " ";
        }
        else if (optiontype == kCOMBO)
        {
            std::cout << "default " << option.get_string() << " ";
            for (std::string s : option.get_combo_options())
                std::cout << "var " << s << " ";
        }
        else if (optiontype == kSTRING)
            std::cout << "default " << option.get_string() << " ";

        std::cout << std::endl;
    }

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
    std::string token;
    istream >> token;

    if (token != "name") return false;

    std::string name = "";

    while (istream >> token && token != "value") name += token + " ";
    name.pop_back();  // remove last space

    if (options.find(name) == options.end()) return false;

    OptionType optiontype = options[name].get_type();

    if (optiontype == kCHECK)
    {
        istream >> token;
        options[name].set(token == "true");
    }
    else if (optiontype == kSPIN)
    {
        int value;
        istream >> value;
        options[name].set(value);
    }
    else if (optiontype == kCOMBO)
    {
        istream >> token;
        options[name].set(token);
    }
    else if (optiontype == kBUTTON)
    {
        options[name].set();
    }
    else if (optiontype == kSTRING)
    {
        istream >> token;
        options[name].set(token);
    }

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
        while (istream >> token && token != "moves") fen += token + " ";

        position = Position(fen);
    }
    else
        return false;

    while (istream >> token)
    {
        if (token == "moves") continue;

        position.do_move(position.parse_uci(token));
    }

    return true;
}

bool Uci::moves_command(std::istringstream& istream)
{
    std::string token;
    while (istream >> token)
    {
        position.do_move(position.parse_uci(token));
    }

    return true;
}

void start_searching(Uci* uci)
{
    uint64_t key = PolyglotBook::hash(uci->position);
    if (uci->polyglot.contains(key))
    {
        Move move = uci->polyglot.sample_move(key, uci->position);
        std::cout << "bestmove " << uci->position.uci(move) << std::endl;
    }
    else
        uci->search->go();
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
                limits.searchmoves[limits.searchmovesnum++] =
                    position.parse_uci(token);
        }
    }

    search = std::make_shared<Search>(position, limits);

    std::thread search_thread(start_searching, this);
    search_thread.detach();

    return true;
}

bool Uci::stop_command(std::istringstream& istream)
{
    if (search) search->stop();
    return true;
}

bool Uci::ponderhit_command(std::istringstream& istream)
{
    return true;
}

bool Uci::quit_command(std::istringstream& istream)
{
    if (search) search->stop();
    quit = true;
    return true;
}

bool Uci::printboard_command(std::istringstream& istream)
{
    std::cout << position << std::endl;
    return true;
}

bool Uci::hash_command(std::istringstream& istream)
{
    std::cout << "Hex: " << std::hex << position.hash() << std::dec
              << std::endl;
    return true;
}

bool Uci::perft_command(std::istringstream& istream)
{
    int depth;
    istream >> depth;

    TimePoint start_time = std::chrono::steady_clock::now();

    uint64_t sum = 0;
    if (depth > 0)
    {
        Move* begin = MOVE_LIST[depth];
        Move* end = generate_moves(position, position.color(), begin);

        for (Move* it = begin; it != end; ++it)
        {
            Move move = *it;
            MoveInfo moveinfo = position.do_move(move);

            uint64_t n = perft(position, depth - 1);

            position.undo_move(move, moveinfo);

            std::cout << position.uci(move) << ": " << n << std::endl;
            sum += n;
        }
    }

    TimePoint end_time = std::chrono::steady_clock::now();
    uint64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                            end_time - start_time)
                            .count();

    std::cout << std::endl;
    std::cout << "Number of nodes: " << sum << std::endl;
    std::cout << "Time: " << duration << "ms" << std::endl;
    std::cout << "Speed: " << sum * 1000LL / (duration + 1) << "nps"
              << std::endl;

    return true;
}

}  // namespace engine
