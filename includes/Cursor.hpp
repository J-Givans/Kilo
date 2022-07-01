#ifndef CURSOR_HPP
#define CURSOR_HPP

#include "Window.hpp"

struct Cursor {
    int x;
    int y;

    void moveCursor(int const key, Window const& window);
};


#endif