#include "Editor.hpp"

#include <cstdlib>
#include <stdexcept>

int main(int argc, char* argv[])
{
    Editor& editor = Editor::instance();

    if (argc >= 2) {
        try {
            editor.open(argv[1]);
        }
        catch (std::runtime_error const& err) {
            std::cerr << err.what() << '\n';
            return EXIT_FAILURE;
        }
    }

    while (true) {
        editor.refreshScreen();
        editor.processKeypress();
    }

    return EXIT_SUCCESS;
}