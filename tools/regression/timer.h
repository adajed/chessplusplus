#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>

class Timer
{
  public:
    using Time = std::chrono::milliseconds::rep;
    using TimePoint = std::chrono::steady_clock::time_point;

    Timer(Time initial, Time increment)
        : initial_(initial),
          increment_(increment),
          time_left_(initial),
          start_time_point_(),
          end_time_point_()
    {
    }

    int get_time_left_ms() { return static_cast<int>(time_left_); }
    int get_time_inc_ms() { return static_cast<int>(increment_); }

    /*
     * Returns time left in human-readable format.
     * Format: hh:mm:ss
     */
    std::string get_time_left_human()
    {
        const int SECOND_TO_MS = 1000;
        const int MINUTE_TO_MS = 60 * SECOND_TO_MS;
        const int HOUR_TO_MS = 60 * MINUTE_TO_MS;
        int time_left_ms = get_time_left_ms();
        int hours_left = time_left_ms / HOUR_TO_MS;
        time_left_ms -= hours_left * HOUR_TO_MS;
        int minutes_left = time_left_ms / MINUTE_TO_MS;
        time_left_ms -= minutes_left * MINUTE_TO_MS;
        int seconds_left = time_left_ms / SECOND_TO_MS;

        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << std::to_string(hours_left) << ":"
           << std::setw(2) << std::setfill('0') << std::to_string(minutes_left) << ":"
           << std::setw(2) << std::setfill('0') << std::to_string(seconds_left);
        return ss.str();
    }

    void start() { start_time_point_ = std::chrono::steady_clock::now(); }

    void end()
    {
        end_time_point_ = std::chrono::steady_clock::now();
        duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(
                        end_time_point_ - start_time_point_)
                        .count();

        if (duration_ > time_left_)
        {
            time_left_ = 0;
        }
        else
        {
            time_left_ = time_left_ - duration_ + increment_;
        }
    }

  private:
    Time initial_, increment_;
    Time time_left_;

    TimePoint start_time_point_, end_time_point_;
    Time duration_;
};
