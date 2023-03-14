#include "lib.hpp"

#include <cstddef>
#include <system_error>
#include <termios.h>
#include <unistd.h>
#include <utility>

/// Wrapper function for tcgetattr
void posix::tcgetattr(int fd, termios& termios)
{
    errno = 0;

    if (std::error_code ec {}; ::tcgetattr(fd, &termios) == -1) {
        ec.assign(errno, std::system_category());
        throw std::system_error {ec, "Failed to get terminal attributes."};
    }
}

/// Wrapper function for tcsetattr
void posix::tcsetattr(int fd, int optionalActions, termios& termios)
{
    errno = 0;

    if (std::error_code ec {}; ::tcsetattr(fd, optionalActions, &termios) == -1) {
        ec.assign(errno, std::system_category());
        throw std::system_error {ec, "Failed to set terminal attributes."};
    }
}

/// Wrapper for POSIX read()
std::size_t posix::read(int fd, void* buffer, std::size_t count)
{
    std::error_code ec;

    if (count > constants::ssize_max) {
        ec.assign(EINVAL, std::system_category());
        throw std::system_error {ec, "Bytes to write greater than write capacity."};
    }

    errno = 0;
    auto read { ::read(fd, buffer, count) };

    if (read < 0 && errno != EAGAIN) {
        ec.assign(errno, std::system_category());
        throw std::system_error {ec};
    }

    return static_cast<std::size_t>(read);
}

/// Wrapper for POSIX write()
std::size_t posix::write(int fd, void const* buffer, std::size_t count)
{
    if (count > constants::ssize_max) {
        throw std::system_error { EINVAL, std::system_category() };
    }

    errno = 0;
    auto written { ::write(fd, buffer, count) };

    if (written < 0) {
        throw std::system_error { errno, std::system_category() };
    }
    else if (static_cast<std::size_t>(written) < count) {
        throw std::system_error { ENOSPC, std::system_category() };
    }

    return static_cast<std::size_t>(written);
}