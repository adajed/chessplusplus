#include "time_manager.h"

#include <cmath>

namespace engine
{

constexpr int MAX_MOVES_TO_GO = 50;

TimeManager::TimeManager(const Limits& limits, Color side, int ply)
    : _time()
{
    int maxMovesToGo = limits.movestogo == 0 ? MAX_MOVES_TO_GO : limits.movestogo;

    int our_time = limits.timeleft[side];
    int our_inc  = limits.timeinc[side];

    _time = Duration(our_time);

    for (int movesToGo = 1; movesToGo < maxMovesToGo; ++movesToGo)
    {
        Duration myTotalTime = our_time + our_inc * (movesToGo - 1);

        Duration t = calculateTime(myTotalTime, movesToGo, ply);

        _time = std::min(_time, t);
    }
};

double importance(double x)
{
    // taken from stockfish
    return std::max(pow(1. + exp((x - 64.5) / 6.85), -0.171), 0.01);
}

Duration TimeManager::calculateTime(Duration totalTime, int movesToGo, int ply) const
{
    double moveImportance = importance(static_cast<double>(ply));
    double restImportance = 0.f;

    for (int i = 1; i < movesToGo; ++i)
        restImportance += importance(static_cast<double>(ply + 2 * i));

    double ratio = moveImportance / (moveImportance + restImportance);

    return Duration(static_cast<double>(totalTime) * ratio);

}


}  // namespace engine
