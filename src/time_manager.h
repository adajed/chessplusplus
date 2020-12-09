#ifndef CHESS_ENGINE_TIME_MANAGER_H_
#define CHESS_ENGINE_TIME_MANAGER_H_

#include "types.h"
#include "movegen.h"

#include <chrono>

namespace engine
{

using Duration = std::chrono::milliseconds::rep;

class TimeManager
{
public:
    TimeManager(const Limits& limits, Color side, int ply);

    Duration getTime() const { return _time; };

private:

    Duration calculateTime(Duration totalTime, int movesToGo, int ply) const;

    Duration _time;
};

} // namespace engine

#endif  // CHESS_ENGINE_TIME_MANAGER_H_
