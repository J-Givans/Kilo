#include "Editor.hpp"

#include <cstdlib>

int main()
{
    Editor kilo {};

    while (true) {
        kilo.refreshScreen();
        kilo.processKeypress();
    }

    return EXIT_SUCCESS;
}