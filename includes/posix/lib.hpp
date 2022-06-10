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
std::size_t read(int fd, void* buf, size_t nbytes);

}

namespace posix::constants {
    constexpr int ssize_max = 0x7ffff000;
}

#endif