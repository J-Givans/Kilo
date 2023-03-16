#include "winsize.hpp"
#include "ioctl/ioctl.hpp"

#include <sys/ioctl.h>
#include <unistd.h>

#include <system_error>
#include <iostream>

namespace kilo::lib::winsize
{
    winsize::winsize() noexcept : row(getSize().first), col(getSize().second)
    {
    }

    std::pair<unsigned short, unsigned short> winsize::getSize() const& noexcept
    {
        struct ::winsize const win {};

        try {
            lib::ioctl::ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
        }
        catch (std::system_error const& err) {
            std::cerr << "Caught exception: " << err.what() << '\n';
            return std::make_pair(0, 0);
        }

        return std::make_pair(win.ws_row, win.ws_col);
    }
}