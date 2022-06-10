#include "Editor.hpp"

#include <cstdlib>
#include <system_error>

int main()
{
    Editor kilo {};

    while (true) {
        kilo.processKeypress();
    }

    return EXIT_SUCCESS;
}