#include "write.hpp"

#include <cstring>
#include <system_error>
#include <cerrno>

#include <unistd.h>

namespace kilo::lib::write
{
    [[nodiscard]] long write(int fd, void const* buffer, std::size_t count)
    {
        if (count > 0x7FFFF000) {
            throw std::system_error(EINVAL, std::generic_category(), "Bytes to read exceed the maximum legal limit.");
        }

        errno = 0;
        auto const rv = ::write(fd, buffer, count);

        if (rv < 0) {
            throw std::system_error(errno, std::generic_category(), std::strerror(errno));
        }
        else if (static_cast<std::size_t>(rv) < count) {
            throw std::system_error(ENOSPC, std::generic_category(), std::strerror(ENOSPC));
        }

        return rv;
    }
}