#include "Terminal.hpp"
#include "posix/lib.hpp"

#include <cstdlib>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <system_error>
#include <iostream>
#include <utility>

void Terminal::enableRawMode()
{
    posix::tcgetattr(STDIN_FILENO, m_terminal);

    struct termios copy {m_terminal};

    // Ignore BREAK condition, no SIGINT on BREAK condition, don't mark parity condition, don't strip 8th bit from input
    // Don't map NL to CR on input, don't ignore CR on input, don't map CR to NL on input, disable start/stop output flow ctrl
    copy.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    
    // Disable all output processing
    copy.c_oflag &= ~(OPOST);

    // Disable echoing (except for new lines), canonical mode, signal chars, and extended input processing 
    copy.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    // Set 8 bits/char
    copy.c_cflag |= (CS8);

    // Read with timeout; read() returns as soon as at least 1 byte is abailable, or when TIME tenths of a second have elapsed
    copy.c_cc[VMIN] = 0;
    copy.c_cc[VTIME] = 1;

    posix::tcsetattr(STDIN_FILENO, TCSAFLUSH, copy);
}

void Terminal::disableRawMode()
{
    posix::tcsetattr(STDIN_FILENO, TCSAFLUSH, m_terminal);
}

/// Attempts to set the terminal attributes to raw mode during construction
/// If construction fails then we exit the program with a fail status code as it is in an invalid state
Terminal::Terminal()
{
    try {
        enableRawMode();
    }
    catch (std::system_error const& err) {
        std::cerr << err.what() << "\r\n";
        std::cerr << err.code() << ": " << err.code().message() << "\r\n";

        std::exit(EXIT_FAILURE);
    }
}

Terminal::~Terminal()
{
    try {
        disableRawMode();
    }
    catch (std::system_error const& err) {
        std::cerr << err.what() << "\r\n";
        std::cerr << err.code() << ": " << err.code().message() << "\r\n";

        std::exit(EXIT_FAILURE);
    }
}