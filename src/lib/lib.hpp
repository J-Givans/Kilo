#ifndef LIB_HPP
#define LIB_HPP

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <cerrno>
#include <cstddef>
#include <cstdlib>

#include <iostream>
#include <system_error>
#include <utility>

namespace lib
{
    void tcgetattr(int fd, termios& termios);
    void tcsetattr(int fd, int optionalActions, termios& termios);

    std::size_t read(int fd, void* buffer, std::size_t count);
    std::size_t write(int fd, void const* buffer, std::size_t count);

    template <typename... Args> void ioctl(int fd, unsigned long request, Args&&... args)
    {
        errno = 0;
        auto ret = ::ioctl(fd, request, std::forward<Args>(args)...);

        if (ret == -1) {
            throw std::system_error(errno, std::system_category());
        }
    }

    struct winsize_t 
    {
    public:
        int row;
        int col;

        /// \brief Default constructor
        winsize_t()
        {
            try {
                auto [r, c] = getWindowSize();

                row = r;
                col = c;
            }
            catch (std::system_error const& e) {
                std::cerr << e.code() << ": " << e.code().message() << '\n';
                std::exit(EXIT_FAILURE);
            }
        }

    private:
        /// \brief Get the size of the currently open terminal window
        /// \returns A pair containing the dimensions of the window
        std::pair<int, int> getWindowSize() const
        {
            winsize m_winsize;
            
            lib::ioctl(STDOUT_FILENO, TIOCGWINSZ, &m_winsize);
            return std::make_pair(m_winsize.ws_row, m_winsize.ws_col);
        }
    };
}

namespace lib::constants 
{
    constexpr int ssize_max = 0x7ffff000;
}

#endif