#pragma once

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include <string>
#include <sstream>
#include <vector>

#include "fdstream.h"

#include "types.h"
#include "position.h"

#define CHILD_STDIN_READ pipefds_input_[0]
#define CHILD_STDIN_WRITE pipefds_input_[1]
#define CHILD_STDOUT_READ pipefds_output_[0]
#define CHILD_STDOUT_WRITE pipefds_output_[1]
#define CHILD_STDERR_READ pipefds_error_[0]
#define CHILD_STDERR_WRITE pipefds_error_[1]

using namespace engine;

struct CommandInfoData
{
    int depth;
    std::string score;
    uint64_t nodes;
    uint64_t nps;
    uint64_t time;
    std::vector<std::string> pv;
};

struct CommandBestmoveData
{
    std::string move;
};

struct CommandGoParams
{
    CommandGoParams() : depth(0), movetime(0), time{0, 0}, time_inc{0, 0} {}
    int depth;
    int movetime;
    int time[COLOR_NUM];
    int time_inc[COLOR_NUM];
};

class EngineWrapper
{

    public:
        explicit EngineWrapper(const std::string& command, const std::string& name, bool debug);

        std::string get_name() const { return name_; }

        void uci();
        void ucinewgame();

        void position(const std::string& fen, const std::vector<Move>& moves);

        void quit();

        Move go(const CommandGoParams& params);

        void set_option(const std::string& name, const std::string& value);
        void set_option(const std::string& name);

    private:
        CommandInfoData parse_command_info(const std::string& command);
        CommandBestmoveData parse_command_bestmove(const std::string& command);

        std::string command_;
        std::string name_;
        bool debug_;

        int pipe_status_;
        int pipefds_input_[2];
        int pipefds_output_[2];
        int pipefds_error_[2];

        ofdstream in_stream_;
        ifdstream out_stream_;
        ifdstream err_stream_;

        bool running_;

        Position position_;
};
