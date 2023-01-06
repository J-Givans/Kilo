#include "Editor.hpp"
#include "Keys.hpp"
#include "posix/lib.hpp"
#include "Utils.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <string>
#include <iostream>
#include <optional>

/// Move the cursor in the direction of the arrow key pressed
void Editor::Cursor::moveCursor(int const key)
{   
    std::string* row = (yPos >= Editor::instance().m_numRows) ? nullptr : &Editor::instance().m_text[yPos];

    switch (key) {
    case ARROW_LEFT:
        if (xPos != 0) { 
            xPos--; 
        }
        else if (yPos > 0) {
            yPos--;
            xPos = std::ssize(Editor::instance().m_text[yPos]);
        }
        
        break;
    
    case ARROW_RIGHT:
        if (row and xPos < std::ssize(*row)) {
            xPos++;
        }
        else if (row and xPos == std::ssize(*row)) {
            yPos++;
            xPos = 0;
        }

        break;

    case ARROW_UP:
        if (yPos != 0) { yPos--; }
        break;

    case ARROW_DOWN:
        if (yPos != Editor::instance().m_numRows) { yPos++; }
        break;
    }

    row = (yPos >= Editor::instance().m_numRows) ? nullptr : &Editor::instance().m_text[yPos];

    if (int rowLen = row ? std::ssize(*row) : 0; xPos > rowLen) {
        xPos = rowLen;
    }
}

Editor::Editor()
:   m_cursor{}, m_winsize{}, m_numRows{}, m_text{}, m_offset{}, m_filename{}
{
    m_winsize.row -= 1;
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

/// Map keypresses to editor operations
void Editor::processKeypress()
{
    int c = readKey();

    switch (c) {
    case ctrlKey('q'):
        std::exit(EXIT_SUCCESS);
        break;

    case HOME:
        m_cursor.xPos = 0;
        break;

    case END:
        if (m_cursor.yPos < m_numRows) {
            m_cursor.xPos = std::ssize(m_text[m_cursor.yPos]);
        }

        break;

    case PAGE_UP:
    case PAGE_DOWN: 
    {
        if (c == PAGE_UP) {
            m_cursor.yPos = m_offset.row;
        }
        else if (c == PAGE_DOWN) {
            m_cursor.yPos = m_offset.row + m_winsize.row - 1;

            if (m_cursor.yPos > m_numRows) {
                m_cursor.yPos = m_numRows;
            }
        }

        auto iterations { m_winsize.row };

        while (--iterations) {
            m_cursor.moveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
        }
    } 
    break;

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
    drawStatusBar(buffer);  // draw a blank white status bar of inverted space characters

    buffer << "\x1b[" << (m_cursor.yPos - m_offset.row) + 1 << ";" << (m_cursor.xPos - m_offset.col) + 1 << "H";  // move the cursor to position (y+1, x+1)
    buffer << "\x1b[?25h"; // show the cursor immediately after repainting

    // Reposition the cursor to the top-left corner
    posix::write(STDOUT_FILENO, buffer.str().c_str(), buffer.str().size());
}

/// Draw a column of tildes on the left-hand side of the screen
/// A tilde is drawn at the beginning of any lines that come after the EOF being edited
void Editor::drawRows(std::stringstream& buffer)
{
    for (int y{0}; y < m_winsize.row; ++y) {
        if (auto fileRow = y + m_offset.row; fileRow >= m_numRows) {
            // Display welcome message iff the user doesn't open a file for reading on program start
            if (m_numRows == 0 and y == m_winsize.row / 3) {
                std::string welcome{"Kilo editor -- version "};
                welcome += KILO_VERSION;

                if (std::ssize(welcome) > m_winsize.col) {
                    welcome.resize(m_winsize.col);
                }

                auto padding { (m_winsize.col - welcome.length()) / 2 };

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
            auto strlen = std::ssize(m_text[fileRow]) - m_offset.col;

            if (strlen < 0) {
                m_text[fileRow].resize(0);
            }
            else if (strlen > m_winsize.col) {
                m_text[fileRow].resize(m_winsize.col);
            }

            buffer << m_text[fileRow];
        }

        buffer << "\x1b[K"; // clear lines one at a time
        buffer << "\r\n";
    }
}

/// Open a file and read its contents
void Editor::open(std::filesystem::path const& path)
{
    m_filename = path.c_str();
    std::ifstream inFile{ path};

    if (!inFile) {
        throw std::runtime_error{"Could not open file."};
    }

    std::string text{};

    while (std::getline(inFile, text)) {
        m_text.push_back(text);
        ++m_numRows;
    }
}

/// Check if the cursor has moved outside the visible window
/// If so, adjust m_offset.row so that the cursor is just inside the visible window
void Editor::scroll()
{
    if (m_cursor.yPos < m_offset.row) {
        m_offset.row = m_cursor.yPos;
    }

    if (m_cursor.yPos >= m_offset.row + m_winsize.row) {
        m_offset.row = m_cursor.yPos - m_winsize.row + 1;
    }

    if (m_cursor.xPos < m_offset.col) {
        m_offset.col = m_cursor.xPos;
    }

    if (m_cursor.xPos >= m_offset.col + m_winsize.col) {
        m_offset.col = m_cursor.xPos - m_winsize.col + 1;
    }
}

void Editor::drawStatusBar(std::stringstream& buffer)
{
    buffer << "\x1b[7m";    // switch to inverted colours (black text; white bg)
    int len{0};

    while (len < m_winsize.col) {
        buffer << ' ';
        ++len;
    }

    buffer << "\x1b[m";     // switch to normal formatting (white text; black bg)
}