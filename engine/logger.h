#ifndef CHESS_ENGINE_LOGGER_H_
#define CHESS_ENGINE_LOGGER_H_

#include <fstream>
#include <iostream>
#include <mutex>

enum SyncCout { IO_LOCK, IO_UNLOCK };
std::ostream& operator<< (std::ostream&, SyncCout);

enum LoggerMode { LOGGER_INPUT, LOGGER_OUTPUT };
std::ostream& operator<< (std::ostream&, LoggerMode);

#define sync_cout std::cout << IO_LOCK
#define sync_endl std::endl << IO_UNLOCK
#define logger_sync_out logger << IO_LOCK << LOGGER_OUTPUT

namespace engine
{
class Logger
{
  public:
    Logger(std::string path) : fout()
    {
        if (path != "") fout = std::ofstream(path);
    }

    void close_file() { fout.close(); }

    void open_file(std::string path)
    {
        close_file();
        fout = std::ofstream(path);
    }

    // this is the type of std::cout
    typedef std::basic_ostream<char, std::char_traits<char>> CoutType;

    // this is the function signature of std::endl
    typedef CoutType& (*StandardEndLine)(CoutType&);

    // define an operator<< to take in std::endl
    Logger& operator<<(StandardEndLine manip)
    {
        // call the function, but we cannot return it's value
        if (mode == LOGGER_OUTPUT) manip(std::cout);
        if (fout.is_open()) fout << std::endl;
        printPrompt = true;

        return *this;
    }

    std::ofstream fout;
    LoggerMode mode;
    bool printPrompt = false;
};

extern Logger logger;

template <typename Type>
Logger& operator<<(Logger& logger, Type value)
{
    if (logger.fout.is_open())
    {
        if (logger.printPrompt)
            logger.fout << (logger.mode == LOGGER_INPUT ? "> " : "< ");
        logger.fout << value;
        logger.printPrompt = false;
    }
    if (logger.mode == LOGGER_OUTPUT)
    {
        std::cout << value;
    }
    return logger;
}

template <>
inline Logger& operator<< <LoggerMode>(Logger& logger, LoggerMode mode)
{
    logger.mode = mode;
    return logger;
}

template <>
inline Logger& operator<< <SyncCout>(Logger& logger, SyncCout sc)
{
    static std::mutex m;
    if (logger.mode == LOGGER_OUTPUT)
    {
        if (sc == IO_LOCK)
            m.lock();

        if (sc == IO_UNLOCK)
            m.unlock();
    }
    return logger;
}

inline Logger& endl(Logger& logger)
{
    logger.fout << std::endl;
    return logger;
}

}  // namespace engine

#endif  // CHESS_ENGINE_LOGGER_H_
