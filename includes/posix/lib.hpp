#ifndef LIB_HPP
#define LIB_HPP

#include <cstddef>
#include <termios.h>
#include <unistd.h>

#include <system_error>
#include <cerrno>

namespace posix {

void tcgetattr(int fd, struct termios& term);
void tcsetattr(int fd, int optionalActs, struct termios& term);

std::pair<std::size_t, std::error_code> read(int fd, void* buffer, std::size_t count);

std::pair<std::size_t, std::error_code> write(int fd, void const* buffer, std::size_t count);

}

namespace posix::constants {
    constexpr int ssize_max = 0x7ffff000;
}

#endif