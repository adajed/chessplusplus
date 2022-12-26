#pragma once

#include "eco_codes.h"
#include "position.h"
#include "types.h"
#include <string>
#include <vector>

struct ExtendedMove
{
    engine::Move move;
    std::string eval;
    std::string timeLeft;
    bool isBookMove;
    std::string san;
};

enum class GameResult
{
    DRAW,
    WHITE_WIN,
    BLACK_WIN,
    NO_RESULT
};

class ExtendedPosition : public engine::Position, public std::vector<ExtendedMove>
{
    public:
        ExtendedPosition(std::string whiteName, std::string blackName,
                         int initial_time, int increment_time)
            : engine::Position(), std::vector<ExtendedMove>()
            , whiteName(whiteName), blackName(blackName)
            , gameResult(GameResult::NO_RESULT)
            , time_initial_ms(initial_time), time_increment_ms(increment_time)

        {
        }

        void setGameResult(GameResult result)
        {
            gameResult = result;
            gameResultString = "*";
            if (gameResult == GameResult::DRAW)
                gameResultString = "1/2-1/2";
            else if (gameResult == GameResult::WHITE_WIN)
                gameResultString = "1-0";
            else if (gameResult == GameResult::BLACK_WIN)
                gameResultString = "0-1";
        }

        std::string pgn() const
        {
            eco::Code eco = eco::get(*this);

            std::stringstream ss;
            ss << "[Event \"Regression Test\"]" << std::endl;
            ss << "[Site \"chessplusplus regression framework\"]" << std::endl;
            ss << "[White \"" << whiteName << "\"]" << std::endl;
            ss << "[Black \"" << blackName << "\"]" << std::endl;
            ss << "[TimeControl \"" << time_initial_ms / 60000 << "+"
                                    << time_increment_ms / 1000 << "\"]" << std::endl;
            ss << "[ECO \"" << eco.code << "\"]" << std::endl;
            ss << "[Opening \"" << eco.openingName << "\"]" << std::endl;
            ss << "[Result \"" << gameResultString << "\"]" << std::endl;
            ss << std::endl;

            int moveIndex = 0;
            for (uint32_t i = 0; i < size(); i++)
            {
                ExtendedMove extMove = at(i);

                if (i % 2 == 0)
                    ss << ++moveIndex << ". ";
                ss << extMove.san << " ";

                if (extMove.isBookMove) ss << "{ book } ";

                ss << "{ ";
                if (!extMove.isBookMove)
                    ss << "[%eval " << extMove.eval << "] ";

                ss << "[%clk " << extMove.timeLeft << "] } ";
            }
            ss << gameResultString << std::endl << std::endl;

            return ss.str();
        }

    private:
        std::string whiteName, blackName;
        GameResult gameResult;
        std::string gameResultString;
        int time_initial_ms, time_increment_ms;
};
