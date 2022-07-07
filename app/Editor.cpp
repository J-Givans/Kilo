#include "Editor.hpp"
#include "posix/lib.hpp"

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string_view>
#include <system_error>
#include <utility>

/// Clear the screen and reposition the cursor on destruction 
/// This handles both cases of program termination, that is, EXIT_SUCCESS and EXIT_FAILURE
Editor::~Editor()
{
    // This way, if an error occurs in the middle of rendering the screen,
    // no garbage is left over, and no errors are printed wherever the cursor happens to be
    posix::write(STDOUT_FILENO, "\x1b[2J", 4);  // clear the screen
    posix::write(STDOUT_FILENO, "\x1b[H", 3);   // reposition the cursor to the top-left corner
}

int Editor::readKey()
{
    char c;
    std::size_t read;

    while ((read = posix::read(STDIN_FILENO, &c, 1)) != 1) {
    }

    // If we read an escape character, immediately read 2 more bytes into seq
    // If either of these reads times out, assume the user pressed ESC and return that
    // Otherwise, look to see if the escape sequence if an arrow key sequence
    // If it is, return the corresponding w a s d character, else return ESC
    if (c == '\x1b') {
        char seq[3];

        if (posix::read(STDIN_FILENO, &seq[0], 1) != 1) { return '\x1b'; }
        if (posix::read(STDIN_FILENO, &seq[1], 1) != 1) { return '\x1b'; }

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (posix::read(STDIN_FILENO, &seq[2], 1) != 1) { return '\x1b'; }

                if (seq[2] == '~') {
                    switch (seq[1]) {
                    case '1': return HOME;
                    case '3': return DELETE;
                    case '4': return END;
                    case '5': return PAGE_UP;
                    case '6': return PAGE_DOWN;
                    case '7': return HOME;
                    case '8': return END;
                    }
                }
            }
            else {
                switch (seq[1]) {
                case 'A': return ARROW_UP;
                case 'B': return ARROW_DOWN;
                case 'C': return ARROW_RIGHT;
                case 'D': return ARROW_LEFT;
                case 'H': return HOME;
                case 'F': return END;
                }
            }
        }
        else if (seq[0] == 'O') {
            switch (seq[1]) {
            case 'H': return HOME;
            case 'F': return END;
            }
        }

        return '\x1b';
    }
    else {
        return c;
    }
}

void Editor::processKeypress()
{
    auto [rows, cols] = m_window.getWindowSize();

    int c {readKey()};

    switch (c) {
    case ctrlKey('q'):
        std::exit(EXIT_SUCCESS);
        break;

    case HOME:
        cursor.x = 0;
        break;

    case END:
        cursor.x = cols - 1;
        break;

    case PAGE_UP: case PAGE_DOWN: 
        {
            auto iterations { rows };

            while (--iterations) {
                cursor.moveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN, m_window);
            }
        }
        break;

    case ARROW_UP: case ARROW_DOWN: case ARROW_LEFT: case ARROW_RIGHT:
        cursor.moveCursor(c, m_window);
        break;
    }
}

void Editor::refreshScreen() const
{
    std::string strbuf {};

    strbuf += "\x1b[?25l";  // hide the cursor while repainting
    strbuf += "\x1b[H";     // reposition the cursor to the top-left corner

    drawRows(strbuf);     // draw column of tildes

    char buffer[32];
    std::snprintf(buffer, sizeof buffer, "\x1b[%d;%dH", cursor.y + 1, cursor.x + 1);
    strbuf += buffer;

    strbuf += "\x1b[?25h";  // show the cursor immediately after repainting
    
    // Reposition the cursor to the top-left corner
    posix::write(STDOUT_FILENO, strbuf.c_str(), strbuf.size());
}

/// Draw a column of tildes on the left-hand side of the screen 
/// A tilde is drawn at the beginning of any lines that come after the EOF being edited
void Editor::drawRows(std::string& buffer) const
{   
    auto [rows, columns] = m_window.getWindowSize();

    for (std::size_t y {}; y < rows; ++y) {
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

        if (y < rows - 1) {
            buffer += "\r\n";
        }
    }
}
