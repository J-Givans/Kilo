#ifndef WRITE_HPP
#define WRITE_HPP

#include <cstddef>

namespace kilo::lib::write
{
    /// \brief Write data to an open file.
    /// \param[in] fd A file descriptor referring to an open file.
    /// \param[in] buffer The address of the data to be written.
    /// \param[in] count The number of bytes to be written
    /// \throws std::system_error An error that occurs upon write failure
    /// \returns The number of bytes written
    [[nodiscard]] long write(int fd, void const* buffer, std::size_t count);
}

#endif