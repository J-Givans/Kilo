#include "Editor.hpp"
#include "posix/lib.hpp"

#include <cstddef>
#include <cstdlib>
#include <system_error>

/**
 * Clear the screen and reposition the cursor on destruction
 * This handles both cases of program termination, that is, EXIT_SUCCESS and EXIT_FAILURE
 */
Editor::~Editor()
{
    // This way, if an error occurs in the middle of rendering the screen,
    // no garbage is left over, and no errors are printed wherever the cursor happens to be
    posix::write(STDOUT_FILENO, "\x1b[2J", 4);  // clear the screen
    posix::write(STDOUT_FILENO, "\x1b[H", 3);   // reposition the cursor to the top-left corner
}

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
    posix::write(STDOUT_FILENO, "\x1b[H", 3);   // reposition the cursor to the top-left corner

    drawRows();     // draw column of tildes

    posix::write(STDOUT_FILENO, "\x1b[H", 3);   // reposition the cursor to the top-left corner
}

/**
 * Draw a column of tildes on the left-hand side of the screen
 * A tilde is drawn at the beginning of any lines that come after the EOF being edited
 */
void Editor::drawRows()
{
    for (int y {}; y < 24; ++y) {
        posix::write(STDOUT_FILENO, "~\r\n", 3);
    }
}