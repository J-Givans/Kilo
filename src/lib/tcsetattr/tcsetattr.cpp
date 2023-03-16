#include "tcsetattr.hpp"

#include <cerrno>
#include <system_error>
#include <cstring>

namespace kilo::lib::tcsetattr
{
    void tcsetattr(int fd, int optionalActions, struct termios const* termios)
    {
        if (errno = 0; termios and ::tcsetattr(fd, optionalActions, termios) == -1) {
            throw std::system_error(errno, std::generic_category(), std::strerror(errno));
        } 
    }
}