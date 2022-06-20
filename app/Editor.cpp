#include "Editor.hpp"
#include "posix/lib.hpp"

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string_view>
#include <system_error>
#include <utility>

Editor::Editor()
{
    try {
        init();       
    }
    catch (std::system_error const& err) {
        std::cerr << err.code() << ": " << err.code().message() << '\n';
        std::exit(EXIT_FAILURE);
    }
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
    constexpr char quit { 'q' & 0x1f };

    switch (c) {
    case quit:
        std::exit(EXIT_SUCCESS);
        break;

    case 'w': case 's': case 'a': case 'd':
        moveCursor(c);
    }
}

void Editor::refreshScreen() const
{
    std::string strbuf {};

    strbuf += "\x1b[?25l";  // hide the cursor while repainting
    strbuf += "\x1b[H";     // reposition the cursor to the top-left corner

    drawRows(strbuf);     // draw column of tildes

    char buffer[32];
    std::snprintf(buffer, sizeof buffer, "\x1b[%d;%dH", cursorY + 1, cursorX + 1);
    strbuf += buffer;

    strbuf += "\x1b[?25h";  // show the cursor immediately after repainting
    
    // Reposition the cursor to the top-left corner
    posix::write(STDOUT_FILENO, strbuf.c_str(), strbuf.size());
}

/// Draw a column of tildes on the left-hand side of the screen 
/// A tilde is drawn at the beginning of any lines that come after the EOF being edited
void Editor::drawRows(std::string& buffer) const
{   
    auto [rows, columns] = m_windowDimensions;

    for (int y {}; y < rows; ++y) {
        if (y == rows / 3) {
            std::string welcome {"Kilo editor -- version "};
            welcome += KILO_VERSION;

            if (welcome.size() > columns) {
                welcome.resize(columns);
            }

            auto padding { (columns - welcome.length()) / 2 };

            if (padding) {
                buffer += '~';
                --padding;
            }

            while (--padding) {
                buffer += " ";
            }

            buffer += welcome;
        }
        else {
            buffer += '~';
        }
    
        buffer += "\x1b[K";     // clear lines one at a time

        if (y < m_windowDimensions.first - 1) {
            buffer += "\r\n";
        }
    }
}

/// Set the Editor's window dimensions by calling Terminal::getWindowSize()
void Editor::init()
{
    m_windowDimensions = Terminal::getWindowSize();
}

void Editor::moveCursor(char const& key)
{
    switch (key) {
    case 'a':
        --cursorX;
        break;

    case 'd':
        ++cursorX;
        break;

    case 'w':
        --cursorY;
        break;

    case 's':
        ++cursorY;
        break;
    }
}