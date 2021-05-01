#ifndef CHESS_ENGINE_LOGGER_H_
#define CHESS_ENGINE_LOGGER_H_

#include <fstream>
#include <iostream>

namespace engine
{

class Logger
{
    public:
        Logger(std::string path) : fout()
        {
            if (path != "")
                fout = std::ofstream(path);
        }

        void close_file()
        {
            fout.close();
        }

        void open_file(std::string path)
        {
            fout.close();
            fout = std::ofstream(path);
        }

        // this is the type of std::cout
        typedef std::basic_ostream<char, std::char_traits<char> > CoutType;

        // this is the function signature of std::endl
        typedef CoutType& (*StandardEndLine)(CoutType&);

        // define an operator<< to take in std::endl
        Logger& operator<<(StandardEndLine manip)
        {
            // call the function, but we cannot return it's value
            manip(std::cout);
            if (fout.is_open())
                fout << std::endl;

            return *this;
        }

        std::ofstream fout;
};

extern Logger logger;

template <typename Type>
Logger& operator<< (Logger& logger, Type value)
{
    if (logger.fout.is_open())
        logger.fout << value;
    std::cout << value;
    return logger;
}


inline Logger& endl(Logger& logger)
{
    logger.fout << std::endl;
    std::cout << std::endl;
    return logger;
}

}

#endif  // CHESS_ENGINE_LOGGER_H_
