#ifndef CHESS_ENGINE_TIME_MANAGER_H_
#define CHESS_ENGINE_TIME_MANAGER_H_

#include "movegen.h"
#include "types.h"

#include <chrono>

namespace engine
{
using Duration = std::chrono::milliseconds::rep;

class TimeManager
{
  public:
    /**
     * Calculate how much time can we spend on this move.
     * \param limits Limits struct with info about 'go' command.
     * \param side Color of current player.
     * \param ply Current move ply.
     */
    static Duration calculateTime(const Limits& limits, Color side, int ply);

  private:
    static Duration computeTimeForFixedLength(Duration totalTime, int movesToGo,
                                              int ply);
};

}  // namespace engine

#endif  // CHESS_ENGINE_TIME_MANAGER_H_
