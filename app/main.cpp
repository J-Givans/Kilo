#include "Editor.hpp"

#include <cstdlib>
#include <stdexcept>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc >= 2) {
        try {
            Editor::instance().open(argv[1]);
        }
        catch (std::runtime_error const& err) {
            std::cerr << err.what() << '\n';
            return EXIT_FAILURE;
        }
    }

    while (true) {
        Editor::instance().refreshScreen();
        Editor::instance().processKeypress();
    }

    return EXIT_SUCCESS;
}