#ifndef WINDOw_HPP
#define WINDOw_HPP

#include <sys/ioctl.h>
#include <utility>

class Window {
public:
    std::pair<std::size_t, std::size_t> getWindowSize() const;

private:
    winsize m_window;
};

#endif