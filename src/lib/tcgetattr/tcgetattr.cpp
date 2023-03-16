#include "tcgetattr.hpp"

#include <cerrno>
#include <system_error>
#include <cstring>

namespace kilo::lib::tcgeatttr
{
    void tcgetattr(int fd, struct termios* term)
    {
        if (errno = 0; term and ::tcgetattr(fd, term) == -1) {
            throw std::system_error(errno, std::generic_category(), std::strerror(errno));
        }
    }   
}