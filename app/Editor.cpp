#include "Editor.hpp"
#include "Keys.hpp"
#include "posix/lib.hpp"
#include "Utils.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include <string>
#include <optional>

#include <fmt/core.h>
#include <fmt/printf.h>

/**
 * @brief Moves the cursor in the direction of the arrow-key pressed
 * @param key One of the four possible arrow-keys
*/
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

/**
 * @brief Default constructor.
 *
 * Creates a default instance of an Editor.
 * Decrements @c m_winsize.row by 1 to create room for the status bar at the bottom of the Editor window.
*/
Editor::Editor()
:   m_cursor{}, m_winsize{}, m_numRows{}, m_text{}, m_offset{}, m_filename{}
{
    m_winsize.row -= 1;
}

/**
 * @brief Clear the screen and reposition the cursor to the top-left corner on program exit.
 *
 * Handles both normal and abnormal program termination.
 * If an error occurs in the middle of rendering the screen, no garbage is left over, and no errors are printed
 * wherever the cursor happens to be.
*/
Editor::~Editor()
{
    try {
        posix::write(STDOUT_FILENO, "\x1b[2J", 4); // clear the screen
        posix::write(STDOUT_FILENO, "\x1b[H", 3); // reposition the cursor to the top-left corner
    }
    catch (std::system_error const& err) {
        fmt::fprintf(stderr, "Error while clearing the screen during program exit.\n");
        std::exit(EXIT_FAILURE);
    }
}

/**
 * @brief Creates a static instance of an Editor
 * @return A reference to a static Editor
*/
Editor& Editor::instance()
{
    static Editor editor{};
    return editor;
}

/** 
 * @brief Maps keypresses to editor operations 
*/
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

/**
 * @brief Handles the painting of TUI elements to the screen.
*/
void Editor::refreshScreen()
{
    scroll();

    std::string buffer;
    buffer += "\x1b[?25l";  // hide the cursor while repainting
    buffer += "\x1b[H";     // reposition the cursor to the top-left corner

    drawRows(buffer);       // draw column of tildes
    drawStatusBar(buffer);  // draw a blank white status bar of inverted space characters

    // Move the cursor to position (y + 1, x + 1)
    std::string str = fmt::format("\x1b[{};{}H", (m_cursor.yPos - m_offset.row) + 1, (m_cursor.xPos - m_offset.col) + 1);
    buffer += str;
    
    // Show the cursor immediately after repainting
    buffer += "\x1b[?25h";

    // Reposition the cursor to the top-left corner
    posix::write(STDOUT_FILENO, buffer.c_str(), buffer.length());
}

/**
 * @brief Draw a column of tildes on the left-hand side of the screen
 *
 * Displays the welcome message if the user doesn't open a file
 * A tilde is drawn at the beginning of any lines that come after the EOF being edited
*/
void Editor::drawRows(std::string& buffer)
{
    for (int y = 0; y < m_winsize.row; ++y) {
        if (int filerow = y + m_offset.row; filerow >= m_numRows) {
            
            // Display the welcome msg if the user doesn't open a file
            if (m_numRows == 0 and y == m_winsize.row / 3) {
                std::string welcome = fmt::format("Kilo Editor --version {}", KILO_VERSION);

                if (std::ssize(welcome) > m_winsize.col) {
                    welcome.resize(m_winsize.col);
                }

                auto padding = (m_winsize.col - welcome.length()) / 2;

                if (padding) {
                    buffer += "~";
                    --padding;
                }

                while (--padding) {
                    buffer += " ";
                }

                buffer += welcome;
            }
            else {
                buffer += "~";
            }
        }
        else {
            auto strlen = std::ssize(m_text[filerow]) - m_offset.col;

            if (strlen < 0) {
                m_text[filerow].resize(0);
            }
            else if (strlen > m_winsize.col) {
                m_text[filerow].resize(m_winsize.col);
            }

            buffer += m_text[filerow];
        }

        buffer += "\x1b[K"; // clear lines one at a time
        buffer += "\r\n";
    }
}

/**
 * @brief Open a file and read its contents
 * @param path A path to the file to be opened.
 *
 * Attempts to open a file for input. Throws a @c std::runtime_error exception on failure
 * Sets the @c m_filename member to the name of the opened file
 * Writes the contents of the file to a vector of strings
 * Increments @c m_numRows to match the number of rows in the opened file
*/
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

/**
 * @brief Determine the position of the cursor within the visible window
 *
 * Checks if the cursor is still within the visible window. 
 * If not, it adjusts @c m_offset.row to reposition it within the visible window
*/
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

/**
 * @brief Draws a status bar at the bottom of the editor window
 * @param buffer The string to which the contents of the status bar are written
*/
void Editor::drawStatusBar(std::string& buffer) const
{
    buffer += "\x1b[7m";    // switch to inverted colours (black text, white background)
    
    std::string status = fmt::sprintf("%.20s - %d lines", m_filename.empty() ? "[No Name]" : m_filename, m_numRows);
    int len = std::ssize(status);

    if (len > m_winsize.col) {
        len = m_winsize.col;
    }

    buffer += status;

    while (len < m_winsize.col) {
        buffer += " ";
        ++len;
    }

    buffer += "\x1b[m";     // switch to normal formatting (white text; black background)
}