#ifndef CHESS_ENGINE_UCI_H_
#define CHESS_ENGINE_UCI_H_

#include "polyglot.h"
#include "position.h"
#include "score.h"
#include "search.h"
#include "transposition_table.h"
#include "ucioption.h"

#include <map>
#include <memory>
#include <sstream>

namespace engine
{
class Uci
{
  public:
    const std::string STARTPOS_FEN =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    Uci();

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

    bool staticeval_command(std::istringstream& istream);

    std::shared_ptr<Search> search;
    Position position;
    PositionScorer scorer;
    tt::TTable ttable;
    bool is_search;
    bool quit;

    std::map<std::string, UciOption> options;
    PolyglotBook polyglot;
    bool polyglot_sample_random_move;

    friend void start_searching(Uci* uci);
};

}  // namespace engine

#endif  // CHESS_ENGINE_UCI_H_
