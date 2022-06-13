#include "Editor.hpp"

#include <cstdlib>

int main()
{
    static Editor kilo {};
    
    Editor::init(kilo);

    while (true) {
        kilo.refreshScreen();
        Editor::processKeypress();
    }

    return EXIT_SUCCESS;
}