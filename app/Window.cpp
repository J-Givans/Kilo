#include "Window.hpp"

#include <cstddef>
#include <sys/ioctl.h>
#include <unistd.h>
#include <utility>
#include <cerrno>
#include <system_error>

std::pair<std::size_t, std::size_t> Window::getWindowSize() const
{
    if (errno = 0; ::ioctl(STDOUT_FILENO, TIOCGWINSZ, &m_window) == -1) {
        throw std::system_error { errno, std::system_category() };
    }

    return std::make_pair(m_window.ws_row, m_window.ws_col);
}