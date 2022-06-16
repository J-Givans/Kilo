#ifndef LIB_HPP
#define LIB_HPP

#include <cstddef>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <system_error>
#include <cerrno>

namespace posix {

    bool tcgetattr(int fd, struct termios& terminal, std::error_code& ec);
    bool tcsetattr(int fd, int optionalActions, struct termios& terminal, std::error_code& ec);

    std::pair<std::size_t, std::error_code> read(int fd, void* buffer, std::size_t count) noexcept;

    std::pair<std::size_t, std::error_code> write(int fd, void const* buffer, std::size_t count) noexcept;

    std::size_t read(int fd, void* buffer, std::size_t count, std::error_code& ec);
    std::size_t write(int fd, void const* buffer, std::size_t count, std::error_code& ec);
}

namespace posix::constants {
    constexpr int ssize_max = 0x7ffff000;
}

#endif