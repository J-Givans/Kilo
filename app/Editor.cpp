#include "Editor.hpp"
#include "posix/lib.hpp"

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <system_error>
#include <utility>

Editor::Editor()
{
    init();
}

/// Clear the screen and reposition the cursor on destruction 
/// This handles both cases of program termination, that is, EXIT_SUCCESS and EXIT_FAILURE
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

void Editor::refreshScreen() const
{
    posix::write(STDOUT_FILENO, "\x1b[2J", 4);  // clear the screen
    posix::write(STDOUT_FILENO, "\x1b[H", 3);   // reposition the cursor to the top-left corner

    drawRows();     // draw column of tildes

    posix::write(STDOUT_FILENO, "\x1b[H", 3);   // reposition the cursor to the top-left corner
}

/// Draw a column of tildes on the left-hand side of the screen 
/// A tilde is drawn at the beginning of any lines that come after the EOF being edited
void Editor::drawRows() const
{   
    for (int y {}; y < m_windowDimensions.first; ++y) {
        posix::write(STDOUT_FILENO, "~", 1);

        if (y < m_windowDimensions.first - 1) {
            posix::write(STDOUT_FILENO, "\r\n", 2);
        }
    }
}

/// Set the Editor's window dimensions by calling Terminal::getWindowSize()
void Editor::init()
{
    try {
        m_windowDimensions = Terminal::getWindowSize();
    }
    catch (std::system_error const& err) {
        std::cerr << err.code() << ": " << err.code().message() << '\n';
        std::cerr << "Failed to set Editor window dimensions.\n";
        std::exit(EXIT_FAILURE);
    }
}