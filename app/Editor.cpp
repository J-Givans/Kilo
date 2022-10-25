#include "Editor.hpp"
#include "posix/lib.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include <string>
#include <iostream>

/// Move the cursor in the direction of the arrow key pressed
void Editor::Cursor::moveCursor(int const key)
{   
    std::string* row = (yPos >= Editor::instance().m_numRows) ? nullptr : &Editor::instance().m_rowsOfText[yPos];

    switch (key) {
    case ARROW_LEFT:
        if (xPos != 0) { xPos--; }
        break;
    
    case ARROW_RIGHT:
        if (row and xPos < std::ssize(*row)) {
            xPos++;
        }
        break;

    case ARROW_UP:
        if (yPos != 0) { yPos--; }
        break;

    case ARROW_DOWN:
        if  (yPos != Editor::instance().m_numRows) { yPos++; }
        break;
    }

    row = (yPos >= Editor::instance().m_numRows) ? nullptr : &Editor::instance().m_rowsOfText[yPos];

    if (int rowLen = row ? std::ssize(*row) : 0; xPos > rowLen) {
        xPos = rowLen;
    }
}

/// Clear the screen and reposition the cursor on destruction
/// This handles both cases of program termination, that is, EXIT_SUCCESS and EXIT_FAILURE
/// This way, if an error occurs in the middle of rendering the screen,
/// no garbage is left over, and no errors are printed wherever the cursor happens to be
Editor::~Editor()
{
    try {
        posix::write(STDOUT_FILENO, "\x1b[2J", 4); // clear the screen
        posix::write(STDOUT_FILENO, "\x1b[H", 3); // reposition the cursor to the top-left corner
    }
    catch (std::system_error const& err) {
        std::cerr << "Error while clearing the screen during program end.\n";
        std::exit(EXIT_FAILURE);
    }
}

/// Create a static instance of the Editor class
/// This is a Meyer's singleton
Editor& Editor::instance()
{
    static Editor editor{};
    return editor;
}

/// Perform low-level keypress handling
int Editor::readKey()
{
    char c{};
    std::size_t read{};

    while ((read = posix::read(STDIN_FILENO, &c, 1)) != 1) {
    }

    // If we read an escape character, immediately read 2 more bytes into seq
    // If either of these reads times out, assume the user pressed ESC and return that
    // Otherwise, look to see if the escape sequence if an arrow key sequence
    // If it is, return the corresponding w a s d character, else return ESC
    if (c == '\x1b') {
        char seq[3];

        if (posix::read(STDIN_FILENO, &seq[0], 1) != 1) {
            return '\x1b';
        }
        if (posix::read(STDIN_FILENO, &seq[1], 1) != 1) {
            return '\x1b';
        }

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (posix::read(STDIN_FILENO, &seq[2], 1) != 1) {
                    return '\x1b';
                }

                if (seq[2] == '~') {
                    switch (seq[1]) {
                    case '1':
                        return HOME;
                    case '3':
                        return DELETE;
                    case '4':
                        return END;
                    case '5':
                        return PAGE_UP;
                    case '6':
                        return PAGE_DOWN;
                    case '7':
                        return HOME;
                    case '8':
                        return END;
                    }
                }
            } 
            else {
                switch (seq[1]) {
                case 'A':
                    return ARROW_UP;
                case 'B':
                    return ARROW_DOWN;
                case 'C':
                    return ARROW_RIGHT;
                case 'D':
                    return ARROW_LEFT;
                case 'H':
                    return HOME;
                case 'F':
                    return END;
                }
            }
        } 
        else if (seq[0] == 'O') {
            switch (seq[1]) {
            case 'H':
                return HOME;
            case 'F':
                return END;
            }
        }

        return '\x1b';
    } 
    else {
        return c;
    }
}

/// Map keypresses to editor operations
void Editor::processKeypress()
{
    auto [rows, cols] = m_winsize.getWindowSize();

    int c = readKey();

    switch (c) {
    case ctrlKey('q'):
        std::exit(EXIT_SUCCESS);
        break;

    case HOME:
        m_cursor.xPos = 0;
        break;

    case END:
        m_cursor.xPos = cols - 1;
        break;

    case PAGE_UP:
    case PAGE_DOWN: {
        auto iterations { rows };

        while (--iterations) {
            m_cursor.moveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
        }
    } break;

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
        m_cursor.moveCursor(c);
        break;
    }
}

/// Print text to the screen
void Editor::refreshScreen()
{
    scroll();

    std::stringstream buffer{};

    buffer << "\x1b[?25l"; // hide the cursor while repainting
    buffer << "\x1b[H"; // reposition the cursor to the top-left corner

    drawRows(buffer); // draw column of tildes

    buffer << "\x1b[" << (m_cursor.yPos - m_rowOffset) + 1 << ";" << (m_cursor.xPos - m_colOff) + 1 << "H";  // move the cursor to position (y+1, x+1)
    buffer << "\x1b[?25h"; // show the cursor immediately after repainting

    // Reposition the cursor to the top-left corner
    posix::write(STDOUT_FILENO, buffer.str().c_str(), buffer.str().size());
}

/// Draw a column of tildes on the left-hand side of the screen
/// A tilde is drawn at the beginning of any lines that come after the EOF being edited
void Editor::drawRows(std::stringstream& buffer)
{
    auto [rows, columns] = m_winsize.getWindowSize();

    for (int y{0}; y < rows; ++y) {
        if (auto fileRow = y + m_rowOffset; fileRow >= m_numRows) {
            // Display welcome message iff the user doesn't open a file for reading on program start
            if (m_numRows == 0 and y == rows / 3) {
                std::string welcome{"Kilo editor -- version "};
                welcome += KILO_VERSION;

                if (std::ssize(welcome) > columns) {
                    welcome.resize(columns);
                }

                auto padding { (columns - welcome.length()) / 2 };

                if (padding) {
                    buffer << '~';
                    --padding;
                }

                while (--padding) {
                    buffer << " ";
                }

                buffer << welcome;
            } 
            else {
                buffer << '~';
            }
        }
        else {
            auto strlen = std::ssize(m_rowsOfText[fileRow]) - m_colOff;

            if (strlen < 0) {
                m_rowsOfText[fileRow].resize(0);
            }
            else if (strlen > columns) {
                m_rowsOfText[fileRow].resize(columns);
            }

            buffer << m_rowsOfText[fileRow];
        }

        buffer << "\x1b[K"; // clear lines one at a time

        if (y < rows - 1) {
            buffer << "\r\n";
        }
    }
}

/// Open a file and read its contents
void Editor::open(std::filesystem::path const& path)
{
    std::ifstream inFile{ path};

    if (!inFile) {
        throw std::runtime_error{"Could not open file."};
    }

    std::string text{};

    while (std::getline(inFile, text)) {
        m_rowsOfText.push_back(text);
        ++m_numRows;
    }
}

/// Check if the cursor has moved outside the visible window
/// If so, adjust m_rowOffset so that the cursor is just inside the visible window
void Editor::scroll()
{
    auto [screenRows, screenCols] = m_winsize.getWindowSize();

    if (m_cursor.yPos < m_rowOffset) {
        m_rowOffset = m_cursor.yPos;
    }

    if (m_cursor.yPos >= m_rowOffset + screenRows) {
        m_rowOffset = m_cursor.yPos - screenRows + 1;
    }

    if (m_cursor.xPos < m_colOff) {
        m_colOff = m_cursor.xPos;
    }

    if (m_cursor.xPos >= m_colOff + screenCols) {
        m_colOff = m_cursor.xPos - screenCols + 1;
    }
}