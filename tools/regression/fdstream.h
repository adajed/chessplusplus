/* This is a basic example of creating a C++ stream from a file
 * descriptor and how to read/write from/to it
 *
 * Needs GCC's libstdc++ and a POSIX environment
 *
 * Author: Diego Barrios Romero
 * Public domain Copyleft 2011
 */

#pragma once

#include <iostream>
#if defined(__GLIBCXX__) || defined(__GLIBCPP__)
#include <ext/stdio_filebuf.h>
#include <unistd.h>

#define BUFFER_SIZE (1024)

class _fdstream
{
  protected:
    _fdstream() : _filebuf(NULL), _file_descriptor(-1) {}

    _fdstream(int file_descriptor, std::ios_base::openmode openmode)
        : _filebuf(NULL), _file_descriptor(file_descriptor), _openmode(openmode)
    {
        open(file_descriptor, openmode);
    }

    void set(int file_descriptor, std::ios_base::openmode openmode)
    {
        delete _filebuf;
        close(_file_descriptor);
        _filebuf = NULL;
        _file_descriptor = file_descriptor;
        _openmode = openmode;
        open(file_descriptor, openmode);
    }

    std::ios_base::openmode openmode() const { return _openmode; }

    void open(int file_descriptor, std::ios_base::openmode openmode)
    {
        if (!_filebuf)
            // We create a C++ stream from a file descriptor
            // stdio_filebuf is not synced with stdio.
            // From GCC 3.4.0 on exists in addition stdio_sync_filebuf
            // You can also create the filebuf from a FILE* with
            // FILE* f = fdopen(file_descriptor, mode);
            _filebuf =
                new __gnu_cxx::stdio_filebuf<char>(file_descriptor, openmode);
    }

    virtual ~_fdstream()
    {
        close(_file_descriptor);
        delete _filebuf;
    }

    __gnu_cxx::stdio_filebuf<char>* _filebuf;

  private:
    int _file_descriptor;
    std::ios_base::openmode _openmode;
};

class ifdstream : public _fdstream
{
  public:
    ifdstream() : _fdstream(), _stream(NULL) {}

    ifdstream(int file_descriptor)
        : _fdstream(file_descriptor, std::ios_base::in)
    {
        _stream = new std::istream(_filebuf);
    }

    void set_fd(int file_descriptor)
    {
        delete _stream;
        set(file_descriptor, std::ios_base::in);
        _stream = new std::istream(_filebuf);
    }

    void open(int file_descriptor)
    {
        if (!_stream)
        {
            _fdstream::open(file_descriptor, std::ios_base::in);
            _stream = new std::istream(_filebuf);
        }
    }

    ifdstream& operator>>(std::string& str)
    {
        (*_stream) >> str;

        return *this;
    }

    void getline(char* s, std::streamsize n) { return (getline(s, n, '\n')); }

    void getline(char* s, std::streamsize n, char delim)
    {
        _stream->getline(s, n, delim);
        if (_stream->rdstate() & std::istream::failbit)
        {
            std::cout << "failbit was set" << std::endl;
            if (_stream->eof()) std::cout << "EOF was set" << std::endl;
            exit(1);
        }
    }

    ~ifdstream() { delete _stream; }

  private:
    std::istream* _stream;
};

class ofdstream : public _fdstream
{
  public:
    ofdstream() : _fdstream(), _stream(NULL) {}

    ofdstream(int file_descriptor)
        : _fdstream(file_descriptor, std::ios_base::out)
    {
        _stream = new std::ostream(_filebuf);
    }

    void set_fd(int file_descriptor)
    {
        delete _stream;
        set(file_descriptor, std::ios_base::out);
        _stream = new std::ostream(_filebuf);
    }

    void open(int file_descriptor)
    {
        if (!_stream)
        {
            _fdstream::open(file_descriptor, std::ios_base::out);
            _stream = new std::ostream(_filebuf);
        }
    }

    ofdstream& operator<<(const std::string& str)
    {
        if (_stream->good())
        {
            (*_stream) << str;
        }

        _stream->flush();
        return *this;
    }

    ~ofdstream() { delete _stream; }

  private:
    std::ostream* _stream;
};

inline void getline_fd(ifdstream& ifds, std::string& str)
{
    char tmp[BUFFER_SIZE];
    ifds.getline(tmp, BUFFER_SIZE);
    str = tmp;
}

#else
#error "Not supported"
#endif
