#ifndef CURSOR_HPP
#define CURSOR_HPP

#include "posix/lib.hpp"

struct Cursor {
    int x{};
    int y{};

    void moveCursor(int const key, posix::winsize_t const& window);
};


#endif