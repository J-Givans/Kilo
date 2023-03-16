#ifndef TCSETATTR_HPP
#define TCSETATTR_HPP

#include <termios.h>

namespace kilo::lib::tcsetattr
{
    /// \brief Modify the attributes of a terminal.
    /// \param[in] fd A file descriptor that refers to a terminal.
    /// \param[in] optionalActions Determine when the changes take effect.
    /// \param[in] termios A pointer to a `termios` structure, which records terminal attributes.
    /// \throws std::system_error A description of the error that occurs when a call to tcsetattr fails.
    void tcsetattr(int fd, int optionalActions, struct termios const* termios);
}

#endif