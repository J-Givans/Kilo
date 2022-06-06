#include "lib.hpp"
#include <cstddef>
#include <system_error>
#include <unistd.h>

void posix::tcgetattr(int fd, struct termios& term)
{
    errno = 0;

    if (::tcgetattr(fd, &term) == -1) {
        throw std::system_error {errno, std::system_category()};
    }
}

void posix::tcsetattr(int fd, int optionalActs, struct termios& term)
{
    errno = 0;

    if (::tcsetattr(fd, optionalActs, &term) == -1) {
        throw std::system_error {errno, std::system_category()};
    }
}

std::size_t posix::read(int fd, void* buf, std::size_t size)
{
    ssize_t bytesRead {0};
    errno = 0;
    
    if (size > constants::ssize_max) {
        throw std::system_error {EINVAL, std::system_category()};
    }
   
    bytesRead = ::read(fd, buf, size);

    if (bytesRead < 0) {
        throw std::system_error {errno, std::system_category()};
    }

    return bytesRead;
}