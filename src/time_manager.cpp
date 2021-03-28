#include "time_manager.h"

#include <cmath>

namespace engine
{

constexpr int MAX_MOVES_TO_GO = 50;

Duration TimeManager::calculateTime(const Limits& limits, Color side, int ply)
{
    int maxMovesToGo = limits.movestogo == 0 ? MAX_MOVES_TO_GO : limits.movestogo;

    int our_time = limits.timeleft[side];
    int our_inc  = limits.timeinc[side];

    Duration time(our_time);

    // For each movesToGo assume that game will last this many moves
    //  and calculate how much time can we spend in this case.
    // Then take minimum over all possible movesToGo values
    //  between 1 and maxMovesToGo.
    for (int movesToGo = 1; movesToGo < maxMovesToGo; ++movesToGo)
    {
        Duration ourTotalTime = our_time + our_inc * (movesToGo - 1);
        Duration t = computeTimeForFixedLength(ourTotalTime, movesToGo, ply);
        time = std::min(time, t);
    }

    Duration time_max(0.7 * our_time);

    return std::min(time, time_max);
};

/**
 * Measures how important is to think on move number 'x'.
 * The higher the value the more time should be spend on this move.
 */
double importance(double x)
{
    // taken from stockfish
    return std::max(pow(1. + exp((x - 64.5) / 6.85), -0.171), 0.01);
}

/**
 * Calculate how much time can be spend on this move
 *  if there are only 'movesToGo' moves till the end of the game.
 */
Duration TimeManager::computeTimeForFixedLength(Duration totalTime, int movesToGo, int ply)
{
    double moveImportance = importance(static_cast<double>(ply));
    double restImportance = 0.f;

    for (int i = 1; i < movesToGo; ++i)
        restImportance += importance(static_cast<double>(ply + 2 * i));

    double ratio = moveImportance / (moveImportance + restImportance);

    return Duration(static_cast<double>(totalTime) * ratio);
}


}  // namespace engine
