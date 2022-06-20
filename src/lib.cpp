#include "posix/lib.hpp"

#include <cstddef>
#include <system_error>
#include <termios.h>
#include <unistd.h>
#include <utility>

/** 
 * Wrapper function for the POSIX read()
 * This function does not throw any exceptions and is therefore marked noexcept
*/
std::pair<std::size_t, std::error_code> posix::read(int fd, void* buffer, std::size_t count) noexcept
{
    std::error_code ec {};

    if (count > constants::ssize_max) {
        ec.assign(EINVAL, std::system_category());
    }

    errno = 0;
    ssize_t bytesRead { ::read(fd, buffer, count) };

    if (bytesRead < 0) {
        ec.assign(errno, std::system_category());
        return std::make_pair(0, ec);
    }

    ec = {};
    return std::make_pair(static_cast<std::size_t>(bytesRead), ec);
}


/** 
 * Wrapper function for the POSIX write()
 * This function does not throw any exceptions and is therefore marked noexcept
*/
std::pair<std::size_t, std::error_code> posix::write(int fd, void const* buffer, std::size_t count) noexcept
{
    std::error_code ec {};

    if (count > constants::ssize_max) {
        ec.assign(EINVAL, std::system_category());
        return std::make_pair(0, ec);
    }

    errno = 0;
    ssize_t written { ::write(fd, buffer, count) };

    if (written < 0) {
        ec.assign(errno, std::system_category());
        return std::make_pair(0, ec);
    }
    else if (static_cast<std::size_t>(written) < count) {
        ec.assign(ENOSPC, std::system_category());
    }
    else {
        ec = {};
    }

    return std::make_pair(static_cast<std::size_t>(written), ec);
}

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

/// Wrapper function for POSIX-style read()
std::size_t posix::read(int fd, void* buffer, std::size_t count, std::error_code& ec)
{
    if (count > constants::ssize_max) {
        ec.assign(EINVAL, std::system_category());
    }

    errno = 0;
    ssize_t read { ::read(fd, buffer, count) };

    if (read < 0) {
        ec.assign(errno, std::system_category());
        return 0;
    }

    ec = {};

    return static_cast<std::size_t>(read);
}

/// Wrapper function for POSIX-style write()
std::size_t posix::write(int fd, void const* buffer, std::size_t count, std::error_code& ec)
{
    if (count > constants::ssize_max) {
        ec.assign(EINVAL, std::system_category());
        return 0;
    }

    errno = 0;
    ssize_t written { ::write(fd, buffer, count) };

    if (written < 0) {
        ec.assign(errno, std::system_category());
        return 0;
    }
    else if (static_cast<std::size_t>(written) < count) {
        ec.assign(ENOSPC, std::system_category());
    }
    else {
        ec = {};
    }

    return static_cast<std::size_t>(written);
}
