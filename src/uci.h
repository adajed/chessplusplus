#ifndef CHESS_ENGINE_UCI_H_
#define CHESS_ENGINE_UCI_H_

#include <sstream>

namespace engine
{

struct Limits
{
};

class Uci
{
    public:
        void loop();

    private:

        void uci_command();

        void ucinewgame_command();

        void isready_command();

        void setoption_command(std::istringstream& is);

        void position_command(std::istringstream& is);

        void go_command(std::istringstream& is);

        void stop_command();

        void ponderhit_command();

        void quit_command();
};

}

#endif  // CHESS_ENGINE_UCI_H_
