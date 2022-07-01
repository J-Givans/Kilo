#include "Window.hpp"

#include <sys/ioctl.h>
#include <unistd.h>
#include <utility>
#include <cerrno>
#include <system_error>

std::pair<int, int> Window::getWindowSize(Window const& window)
{
    if (errno = 0; ::ioctl(STDOUT_FILENO, TIOCGWINSZ, &window.m_window) == -1) {
        throw std::system_error { errno, std::system_category() };
    }

    return std::make_pair(window.m_window.ws_row, window.m_window.ws_col);
}