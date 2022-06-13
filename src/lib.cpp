#include "posix/lib.hpp"

#include <cstddef>
#include <system_error>
#include <unistd.h>
#include <utility>

/** 
 * Wrapper function for the POSIX tcgetattr()
 * This function does not throw any exceptions and is therefore marked noexcept
*/
[[nodiscard]]
std::error_code posix::tcgetattr(int fd, struct termios& term) noexcept
{
    std::error_code ec;
    errno = 0;

    if (::tcgetattr(fd, &term) == -1) {
        ec.assign(errno, std::system_category());
    }

    ec = {};
    return ec;
}

/** 
 * Wrapper function for the POSIX tcsetattr()
 * This function does not throw any exceptions and is therefore marked noexcept
*/
[[nodiscard]]
std::error_code posix::tcsetattr(int fd, int optionalActs, struct termios& term) noexcept
{
    std::error_code ec;
    errno = 0;

    if (::tcsetattr(fd, optionalActs, &term) == -1) {
        ec.assign(errno, std::system_category());
    }

    ec = {};
    return ec;
}

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
