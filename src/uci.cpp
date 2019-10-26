#include "uci.h"

#include <iostream>

namespace engine
{

void Uci::loop()
{
    std::cout << "Chess engine by Adam Jędrych"
              << " (build  " << __DATE__ << " " << __TIME__ << ")" << std::endl;
}

void Uci::uci_command()
{
}

void Uci::ucinewgame_command()
{
}

void Uci::isready_command()
{
}

void Uci::setoption_command(std::istringstream& is)
{
}

void Uci::position_command(std::istringstream& is)
{
}

void Uci::go_command(std::istringstream& is)
{
}

void Uci::stop_command()
{
}

void Uci::ponderhit_command()
{
}

void Uci::quit_command()
{
}

}
