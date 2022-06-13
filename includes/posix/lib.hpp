#ifndef LIB_HPP
#define LIB_HPP

#include <cstddef>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <system_error>
#include <cerrno>

namespace posix {

[[nodiscard]]
std::error_code tcgetattr(int fd, struct termios& term) noexcept;

[[nodiscard]]
std::error_code tcsetattr(int fd, int optionalActs, struct termios& term) noexcept;

std::pair<std::size_t, std::error_code> read(int fd, void* buffer, std::size_t count) noexcept;

std::pair<std::size_t, std::error_code> write(int fd, void const* buffer, std::size_t count) noexcept;

}

namespace posix::constants {
    constexpr int ssize_max = 0x7ffff000;
}

#endif