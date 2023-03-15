#ifndef READ_HPP
#define READ_HPP

#include <cstddef>

namespace kilo::lib::read
{
    /// \brief Read data from an open file
    /// \param[in] fd A file descriptor referring to an open file
    /// \param[in] buffer The address of the memory buffer into which the input data is to be placed
    /// \param[in] count The maximum number of bytes to read
    /// \throws std::system_error Description of the variable errno was set to during a call to ::read
    /// \returns The number of bytes read, or 0 if EOF is encountered
    [[nodiscard]] long read(int fd, void* buffer, std::size_t count);
}

#endif