#include "Terminal/Terminal.hpp"
#include <tcgetattr/tcgetattr.hpp>
#include <tcsetattr/tcsetattr.hpp>

#include <cstdlib>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <cerrno>
#include <system_error>
#include <iostream>
#include <utility>

#include <fmt/core.h>
#include <gsl/assert>

void Terminal::enableRawMode()
{
    // We expect to have populated m_terminal with the terminal driver settings
    // Copy these into `copy`, from which we'll make our changes
    termios copy {m_terminal};

    // Ignore BREAK condition, no SIGINT on BREAK condition, don't mark parity condition, don't strip 8th bit from input
    // Don't map NL to CR on input, don't ignore CR on input, don't map CR to NL on input, disable start/stop output flow ctrl
    copy.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    
    // Disable all output processing
    copy.c_oflag &= ~OPOST;

    // Disable echoing (except for new lines), canonical mode, signal chars, and extended input processing 
    copy.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    // Set 8 bits/char
    copy.c_cflag |= CS8;

    // Read with timeout; read() returns as soon as at least 1 byte is available, or when TIME tenths of a second have elapsed
    copy.c_cc[VMIN] = 0;
    copy.c_cc[VTIME] = 1;

    kilo::lib::tcsetattr::tcsetattr(STDIN_FILENO, TCSAFLUSH, &copy);
}

/// \details Query the terminal driver and write its settings to m_terminal
/// \details Exit the program if this fails
Terminal::Terminal()
{
    // Attempt to query the terminal driver and write its settings to m_terminal
    // If this fails, log the error and exit the program with status EXIT_FAILURE
    if (errno = 0; tcgetattr(STDIN_FILENO, &m_terminal) == -1) {
        fmt::print(stderr, "tcgetattr failed: {}", std::strerror(errno));
        std::exit(EXIT_FAILURE);
    }

    Ensures(errno != ENOTTY);
}

/// \details Reset the terminal driver to canonical mode
/// \details Exits the program if this fails
Terminal::~Terminal()
{
    // Attempt to set the terminal driver settings to those in m_terminal
    // If this fails, log the error that occured and exit the program with status EXIT_FAILURE
    if (errno = 0; tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_terminal) == -1) {
        fmt::print(stderr, "tcsetattr failed: {}", std::strerror(errno));
        std::exit(EXIT_FAILURE);
    }

    Ensures(errno != ENOTTY);
}