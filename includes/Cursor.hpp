#ifndef CURSOR_HPP
#define CURSOR_HPP

#include "Window.hpp"

struct Cursor {
    unsigned int x {};
    unsigned int y {};

    void moveCursor(int const key, Window const& window);
};


#endif