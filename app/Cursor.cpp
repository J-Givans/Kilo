#include "Cursor.hpp"
#include "Keys.hpp"
#include "Window.hpp"

void Cursor::moveCursor(int const key, Window const& window)
{
    auto [row, col] = window.getWindowSize();
    
    switch (key) {
    case ARROW_LEFT:
        if (x != 0) { --x; }
        break;
    
    case ARROW_RIGHT:
        if (x != col - 1) { ++x; }
        break;

    case ARROW_UP:
        if (y != 0) { --y; }
        break;

    case ARROW_DOWN:
        if (y != row - 1) { ++y; }
        break;
    }

    return;
}