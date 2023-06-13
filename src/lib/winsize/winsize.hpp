#ifndef WINSIZE_HPP
#define WINSIZE_HPP

#include <utility>

namespace kilo::lib::winsize
{
    struct winsize
    {
        unsigned short row {};
        unsigned short col {};

        /// \brief Default constructor
        /// \brief The row and col members are each initialised with the current terminal window size or 0
        winsize() noexcept;

    private:
        /// \brief Get the size of the open terminal windiw
        /// \returns A pair containing the dimensions of the terminal window
        std::pair<unsigned short, unsigned short> getSize() const& noexcept;
    };
}

#endif