#ifndef TCGETATTR_HPP
#define TCGETATTR_HPP

#include <termios.h>

namespace kilo::lib::tcgeatttr
{
    /// \brief Retrieve the attributes of a terminal
    /// \param[in] fd A file descriptor that refers to a terminal.
    /// \param[in] term A pointer to a `termios` structure, which records terminal attributes
    /// \throws std::system_error The error that occurs when a call to tcgetattr fails.
    void tcgetattr(int fd, struct termios* term);
}

#endif