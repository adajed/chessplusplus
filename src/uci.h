#ifndef CHESS_ENGINE_UCI_H_
#define CHESS_ENGINE_UCI_H_

#include <map>
#include <memory>
#include <sstream>

#include "polyglot.h"
#include "position.h"
#include "search.h"
#include "ucioption.h"

namespace engine
{

class Uci
{
    public:
        const std::string STARTPOS_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

        Uci(const PositionScorer& scorer);

        void loop();

    private:

        bool uci_command(std::istringstream& istream);

        bool ucinewgame_command(std::istringstream& istream);

        bool isready_command(std::istringstream& istream);

        bool setoption_command(std::istringstream& istream);

        bool position_command(std::istringstream& istream);

        bool go_command(std::istringstream& istream);

        bool stop_command(std::istringstream& istream);

        bool ponderhit_command(std::istringstream& istream);

        bool quit_command(std::istringstream& istream);

        bool printboard_command(std::istringstream& istream);

        bool hash_command(std::istringstream& istream);

        bool perft_command(std::istringstream& istream);

        bool moves_command(std::istringstream& istream);

        PositionScorer scorer;
        std::shared_ptr<Search> search;
        Position position;
        bool is_search;
        bool quit;

        std::map<std::string, UciOption> options;
        PolyglotBook polyglot;
};

}

#endif  // CHESS_ENGINE_UCI_H_
