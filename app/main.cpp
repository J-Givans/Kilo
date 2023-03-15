#include "Editor/Editor.hpp"

#include <cstdlib>
#include <stdexcept>

int main(int argc, char* argv[])
{
    Editor& editor = Editor::instance();

    if (argc >= 2) {
        editor.open(argv[1]);
    }

    while (true) {
        editor.refreshScreen();
        editor.processKeypress();
    }

    return EXIT_SUCCESS;
}