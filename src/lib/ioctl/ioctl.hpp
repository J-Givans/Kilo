#ifndef IOCTL_HPP
#define IOCTL_HPP

#include <sys/ioctl.h>

#include <cerrno>
#include <system_error>
#include <cstring>

namespace kilo::lib::ioctl
{
    /// \brief Manipulate the underlying device parameters of special files.
    /// \tparam Args Optional set of arguments
    /// \param[in] fd A file descriptor referring to an open file.
    /// \param[in] request A device-dependent request code.
    /// \param[in] args An untyped pointer to memory
    /// \throws std::system_error An error that occurs when a call to ioctl fails.
    template <typename... Args> void ioctl(int fd, unsigned long request, Args&&... args)
    {
        if (errno = 0; ::ioctl(fd, request, std::forward<Args>(args)...) == -1) {
            throw std::system_error(errno, std::generic_category(), std::strerror(errno));
        }
    }
}

#endif