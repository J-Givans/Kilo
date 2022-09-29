#ifndef LIB_HPP
#define LIB_HPP

#include <asm-generic/ioctls.h>
#include <cstddef>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <cerrno>
#include <system_error>
#include <utility>

namespace posix {
void tcgetattr(int fd, termios& termios);
void tcsetattr(int fd, int optionalActions, termios& termios);

std::size_t read(int fd, void* buffer, std::size_t count);
std::size_t write(int fd, void const* buffer, std::size_t count);

class winsize_t {
public:
    std::pair<int, int> getWindowSize() const
    {
        if (errno = 0; ::ioctl(STDOUT_FILENO, TIOCGWINSZ, &m_winsize) == -1) {
            throw std::system_error { errno, std::system_category() };
        }

        return std::make_pair(m_winsize.ws_row, m_winsize.ws_col);
    }

private:
    winsize m_winsize;
};


}

namespace posix::constants {
constexpr int ssize_max = 0x7ffff000;
}

#endif