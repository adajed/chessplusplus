#pragma once

#include <chrono>

class Timer
{
    public:
        using Time = std::chrono::milliseconds::rep;
        using TimePoint = std::chrono::steady_clock::time_point;

        Timer(Time initial, Time increment)
            : initial_(initial), increment_(increment), time_left_(initial)
            , start_time_point_(), end_time_point_()
        {
        }

        int get_time_left_ms() { return static_cast<int>(time_left_); }
        int get_time_inc_ms() { return static_cast<int>(increment_); }

        void start()
        {
            start_time_point_ = std::chrono::steady_clock::now();
        }

        void end()
        {
            end_time_point_ = std::chrono::steady_clock::now();
            duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_point_ - start_time_point_).count();

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
