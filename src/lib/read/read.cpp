#include "read.hpp"

#include <unistd.h>

#include <system_error>
#include <cerrno>
#include <cstring>

namespace kilo::lib::read
{
    [[nodiscard]] long read(int fd, void* buffer, std::size_t count)
    {
        if (count > 0x7FFFF000) {
            throw std::system_error(EINVAL, std::generic_category(), "The number of bytes to be read exceeds the maximum possible limit");
        }

        errno = 0;
        auto const rv = ::read(fd, buffer, count);

        if (rv < 0 and errno != EAGAIN) {
            throw std::system_error(errno, std::generic_category(), std::strerror(errno));
        }

        return rv;
    }
}