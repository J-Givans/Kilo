#include "Editor.hpp"

#include <cstdlib>
#include <stdexcept>
#include <iostream>

int main(int argc, char* argv[])
{
    static Editor kilo {};

    if (argc >= 2) {
        try {
            kilo.open(argv[1]);
        }
        catch (std::runtime_error const& err) {
            std::cerr << err.what() << '\n';
            return EXIT_FAILURE;
        }
    }

    while (true) {
        kilo.refreshScreen();
        kilo.processKeypress();
    }

    return EXIT_SUCCESS;
}