#include "Editor.hpp"

#include <cstdlib>

int main()
{
    static Editor kilo {};

    while (true) {
        Editor::refreshScreen();
        Editor::processKeypress();
    }

    return EXIT_SUCCESS;
}