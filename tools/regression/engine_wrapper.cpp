#include "engine_wrapper.h"

#include <iomanip>
#include <sstream>

#include "position.h"

#define SEND(msg)                                      \
    {                                                  \
        if (debug_)                                    \
        {                                              \
            std::cout << name_ << "< " << msg << "\n"; \
        }                                              \
        in_stream_ << msg << "\n";                     \
    }

#define READ(msg)                                           \
    {                                                       \
        getline_fd(out_stream_, msg);                       \
        if (debug_)                                         \
        {                                                   \
            std::cout << name_ << "> " << msg << std::endl; \
        }                                                   \
    }

EngineWrapper::EngineWrapper(const std::string& command,
                             const std::string& name, bool debug)
    : command_(command), name_(name), debug_(debug), running_(false)
{
    pipe_status_ = pipe(pipefds_input_);
    if (pipe_status_ == -1)
    {
        perror("Error creating the pipe");
        exit(EXIT_FAILURE);
    }

    pipe_status_ = pipe(pipefds_output_);
    if (pipe_status_ == -1)
    {
        perror("Error creating the pipe");
        exit(EXIT_FAILURE);
    }

    pipe_status_ = pipe(pipefds_error_);
    if (pipe_status_ == -1)
    {
        perror("Error creating the pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    pid = fork();

    if (pid == pid_t(0))
    {
        // child process
        dup2(CHILD_STDIN_READ, 0);
        dup2(CHILD_STDOUT_WRITE, 1);
        dup2(CHILD_STDERR_WRITE, 2);
        // Close in the child the unused ends of the pipes
        close(CHILD_STDIN_WRITE);
        close(CHILD_STDOUT_READ);
        close(CHILD_STDERR_READ);

        // Execute the program
        execl(command_.c_str(), "engine", (char*)NULL);
    }
    else if (pid > pid_t(0))
    {
        // parent process

        // Close in the parent the unused ends of the pipes
        close(CHILD_STDIN_READ);
        close(CHILD_STDOUT_WRITE);
        close(CHILD_STDERR_WRITE);

        in_stream_.set_fd(CHILD_STDIN_WRITE);
        out_stream_.set_fd(CHILD_STDOUT_READ);
        err_stream_.set_fd(CHILD_STDERR_READ);
    }
    else
    {
        perror("Error: fork failed");
        exit(EXIT_FAILURE);
    }
}

void EngineWrapper::uci()
{
    std::string message = "";

    SEND("uci");

    do
    {
        READ(message);
    } while (message != "uciok");
}

void EngineWrapper::ucinewgame()
{
    SEND("ucinewgame");
}

void EngineWrapper::position(const std::string& fen,
                             const std::vector<Move>& moves)
{
    position_ = Position(fen);

    std::string command = "position";
    if (fen == Position::STARTPOS_FEN)
    {
        command += " startpos";
    }
    else
    {
        command += " fen " + fen;
    }

    if (moves.size() > 0)
    {
        command += " moves";
        for (Move move : moves)
        {
            command += " " + position_.uci(move);
            position_.do_move(move);
        }
    }

    SEND(command);
}

void EngineWrapper::quit()
{
    SEND("quit");

    close(CHILD_STDIN_WRITE);
    close(CHILD_STDOUT_READ);
    close(CHILD_STDERR_READ);
}

Move EngineWrapper::go(const CommandGoParams& params, Color color, std::string& score)
{
    if (running_) return NO_MOVE;

    std::string command = "go";
    if (params.depth > 0)
    {
        command += " depth " + std::to_string(params.depth);
    }
    else if (params.movetime > 0)
    {
        command += " movetime " + std::to_string(params.movetime);
    }
    else if (params.time[WHITE] > 0 && params.time[BLACK] > 0)
    {
        command += " wtime " + std::to_string(params.time[WHITE]);
        command += " btime " + std::to_string(params.time[BLACK]);
        if (params.time_inc[WHITE] > 0 && params.time_inc[BLACK] > 0)
        {
            command += " winc " + std::to_string(params.time_inc[WHITE]);
            command += " binc " + std::to_string(params.time_inc[BLACK]);
        }
    }

    SEND(command);

    running_ = true;
    std::string message = "";
    do
    {
        READ(message);
        if (message.find("info") == 0)
        {
            CommandInfoData infoData = parse_command_info(message);
            int value = color == WHITE ? infoData.scoreValue : -infoData.scoreValue;
            if (infoData.scoreType == ScoreType::kCentipawn)
            {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << static_cast<double>(value) / 100.0;
                score = ss.str();
            }
            else if (infoData.scoreType == ScoreType::kMate)
            {
                score = "#" + std::to_string(value);
            }
        }
    } while (message.find("bestmove") != 0);

    CommandBestmoveData data = parse_command_bestmove(message);
    running_ = false;
    return position_.parse_uci(data.move);
}

void EngineWrapper::set_option(const std::string& name,
                               const std::string& value)
{
    SEND("set_option name " << name << " value " << value);
    SEND("isready");
    std::string message = "";
    do
    {
        READ(message);
    } while (message != "readyok");
}

void EngineWrapper::set_option(const std::string& name)
{
    SEND("setoption name " << name);
    SEND("isready");
    std::string message = "";
    do
    {
        READ(message);
    } while (message != "readyok");
}

CommandInfoData EngineWrapper::parse_command_info(const std::string& command)
{
    std::istringstream ss(command);
    std::string token;
    ss >> token;

    CommandInfoData data;

    while (ss >> token)
    {
        if (token == "depth")
        {
            ss >> token;
            data.depth = std::atoi(token.c_str());
        }
        else if (token == "score")
        {
            std::string s;
            ss >> token;
            if (token == "cp")
                data.scoreType = ScoreType::kCentipawn;
            else if (token == "mate")
                data.scoreType = ScoreType::kMate;
            s += token;
            ss >> token;
            data.scoreValue = std::atoi(token.c_str());
        }
        else if (token == "nodes")
        {
            ss >> token;
            data.nodes = std::atoi(token.c_str());
        }
        else if (token == "nps")
        {
            ss >> token;
            data.nps = std::atoi(token.c_str());
        }
        else if (token == "time")
        {
            ss >> token;
            data.time = std::atoi(token.c_str());
        }
        else if (token == "pv")
        {
            while (ss >> token) data.pv.push_back(token.c_str());
        }
    }

    return data;
}

CommandBestmoveData EngineWrapper::parse_command_bestmove(
    const std::string& command)
{
    std::istringstream ss(command);
    std::string token;
    ss >> token;
    ss >> token;

    CommandBestmoveData data;
    data.move = token;
    return data;
}
