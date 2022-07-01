#ifndef WINDOw_HPP
#define WINDOw_HPP

#include <sys/ioctl.h>
#include <utility>

class Window {
public:
    static std::pair<int, int> getWindowSize(Window const& window);

private:
    static winsize m_window;
};

#endif