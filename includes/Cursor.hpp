#ifndef CURSOR_HPP
#define CURSOR_HPP

#include "posix/lib.hpp"

struct Cursor {
    unsigned int x {};
    unsigned int y {};

    void moveCursor(int const key, posix::winsize_t const& window);
};


#endif