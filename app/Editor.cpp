#include "Editor.hpp"
#include "posix/lib.hpp"

#include <cstddef>
#include <cstdlib>
#include <system_error>

char Editor::readKey()
{
    char c;
    std::pair<std::size_t, std::error_code> result;

    do {
        result = posix::read(STDIN_FILENO, &c, 1);

        if (result.second && result.second != std::errc::resource_unavailable_try_again) {
            throw std::system_error {result.second, "read failed"};
        }
    }
    while (result.first != 1);

    return c;
}

void Editor::processKeypress()
{
    char c {readKey()};

    switch (c) {
    case ctrlKey('q'):
        std::exit(EXIT_SUCCESS);
        break;
    }
}

void Editor::refreshScreen()
{
    posix::write(STDOUT_FILENO, "\x1b[2J", 4);  // clear the screen
    posix::write(STDOUT_FILENO, "\x1b[H", 3);   // reposition the cursor
}