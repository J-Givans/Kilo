#include "Editor.hpp"
#include "Keys.hpp"
#include "posix/lib.hpp"
#include "Utils.hpp"

#include <cstddef>
#include <cstdlib>
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
void Cursor::moveCursor(Key const& key)
{   
    auto const& editor = Editor::instance();
    std::string const* row = (yPos >= editor.m_numRows) ? nullptr : &editor.m_text[yPos];

    switch (key) {
    case Key::ArrowLeft:
        if (xPos != 0) { 
            xPos--; 
        }
        else if (yPos > 0) {
            yPos--;
            xPos = std::ssize(editor.m_text[yPos]);
        }
        
        break;
    
    case Key::ArrowRight:
        if (row and xPos < std::ssize(*row)) {
            xPos++;
        }
        else if (row and xPos == std::ssize(*row)) {
            yPos++;
            xPos = 0;
        }

        break;

    case Key::ArrowUp:
        if (yPos != 0) { yPos--; }
        break;

    case Key::ArrowDown:
        if (yPos != editor.m_numRows) { yPos++; }
        break;

    default:
        break;
    }

    row = (yPos >= editor.m_numRows) ? nullptr : &editor.m_text[yPos];

    if (auto rowLen = row ? std::ssize(*row) : 0; xPos > rowLen) {
        xPos = static_cast<int>(rowLen);
    }
}

/**
 * @brief Default constructor.
 *
 * @details Creates a default instance of an Editor.
 * Decrements @c m_winsize.row by 1 to create room for the status bar at the bottom of the Editor window.
*/
Editor::Editor()
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
        fmt::print(stderr, "Error {}\n\tencountered while clearing the screen.\n", err.code().message());
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
    int c;

    try {
        c = readKey();
    }
    catch (std::system_error const& err) {
        fmt::print(stderr, "Error: {}\n", err.code().message());
        return;
    }

    if (c == ctrlKey('q')) {
        std::exit(EXIT_SUCCESS);
    }
    else {
        auto keyPressed = static_cast<Key>(c);
        processKeypressHelper(keyPressed);
    }
}

void Editor::processKeypressHelper(Key const& key) noexcept
{
    auto const& [col, row] = m_offset.getPosition();

    if (isHomeKey(key)) {
        m_cursor.xPos = 0;
    }
    else if (isEndKey(key)) {
        if (m_cursor.yPos < m_numRows) {
            m_cursor.xPos = std::ssize(m_text[m_cursor.yPos]);
        }
    }
    else if (isPageKey(key)) {
        if (key == Key::PageUp) {
            m_cursor.yPos = col;
        }
        else if (key == Key::PageDown) {
            m_cursor.yPos = col + m_winsize.row - 1;

            if (m_cursor.yPos > m_numRows) {
                m_cursor.yPos = m_numRows;
            }
        }

        for (auto iter = m_winsize.row; ; --iter) {
            m_cursor.moveCursor(key == Key::PageUp ? Key::ArrowUp : Key::ArrowDown);
        }
    }
    else if (isArrowKey(key)) {
        m_cursor.moveCursor(key);
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

    auto const& [col, row] = m_offset.getPosition();

    // Move the cursor to position (y + 1, x + 1)
    std::string str = fmt::format("\x1b[{};{}H", (m_cursor.yPos - col) + 1, (m_cursor.xPos - row) + 1);
    buffer += str;
    
    // Show the cursor immediately after repainting
    buffer += "\x1b[?25h";

    // Reposition the cursor to the top-left corner
    posix::write(STDOUT_FILENO, buffer.c_str(), buffer.length());
}

/**
 * @brief Display a welcome message if no file was opened for reading
 * 
 * @param buffer The buffer to which the welcome message is written
 */
void Editor::displayWelcomeMessage(std::string& buffer) const
{
    std::string welcomeMsg = fmt::format("Kilo Editor -- Version {}", KILO_VERSION);

    // If the welcome message is longer than the width of the window, we trim it to fit the window
    if (std::ssize(welcomeMsg) > m_winsize.col) {
        welcomeMsg.resize(static_cast<std::size_t>(m_winsize.col));
    }

    //! @c padding The position from which the welcome message will be printed (it is centered)
    // Equivalent to the distance from the edges to the welcome message
    auto padding = (m_winsize.col - std::ssize(welcomeMsg)) / 2;

    // If the welcome message doesn't fill the row from edge to edge, print a ~
    if (padding > 0) {
        buffer += "~";
        padding -= 1;
    }

    // Add spaces from the end of the welcome string to the edges
    while (padding > 0) {
        buffer += " ";
        padding -= 1;
    }

    buffer += welcomeMsg;
}

/**
 * @brief Draw a column of tildes on the left-hand side of the screen
 *
 * Displays the welcome message if the user doesn't open a file
 * A tilde is drawn at the beginning of any lines that come after the EOF being edited
*/
void Editor::drawRows(std::string& buffer)
{
    auto const& [col, row] = m_offset.getPosition();

    for (int y = 0; y < m_winsize.row; ++y) {
        if (int filerow = y + col; filerow >= m_numRows) {
            
            // Display the welcome msg if the user doesn't open a file
            if (m_numRows == 0 and y == m_winsize.row / 3) {
                displayWelcomeMessage(buffer);
            }
            else {
                buffer += "~";
            }
        }
        else {
            if (auto strlen = std::ssize(m_text[filerow]) - row; strlen < 0) {
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
 * Attempts to open a file for input.
 * Sets the @c m_filename member to the name of the opened file
 * Writes the contents of the file to a vector of strings
 * Increments @c m_numRows to match the number of rows in the opened file
*/
void Editor::open(std::filesystem::path const& path)
{
    m_filename = path.c_str();
    std::ifstream inFile{ path};

    if (!inFile) {
        fmt::print(stderr, "Could not open file {}.\n", m_filename);
        return;
    }

    std::string text{};

    while (std::getline(inFile, text)) {
        m_text.push_back(text);
    }

    m_numRows = static_cast<int>(m_text.size());
}

/**
 * @brief Determine the position of the cursor within the visible window
 *
 * Checks if the cursor is still within the visible window. 
 * If not, it adjusts @c m_offset.row to reposition it within the visible window
*/
void Editor::scroll()
{
    auto& [col, row] = m_offset.getPosition();

    if (m_cursor.yPos < col) {
        col = m_cursor.yPos;
    }

    if (m_cursor.yPos >= col + m_winsize.row) {
        col = m_cursor.yPos - m_winsize.row + 1;
    }

    if (m_cursor.xPos < row) {
        row = m_cursor.xPos;
    }

    if (m_cursor.xPos >= row + m_winsize.col) {
        row = m_cursor.xPos - m_winsize.col + 1;
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
    auto len = std::ssize(status);

    if (len > m_winsize.col) {
        len = m_winsize.col;
        status.resize(static_cast<std::size_t>(len));
    }

    buffer += status;

    std::string rstatus = fmt::format("{}/{}", m_cursor.yPos + 1, m_numRows);

    while (len < m_winsize.col) {
        if (auto rlen = std::ssize(rstatus); m_winsize.col - len == rlen) {
            buffer += rstatus;
            break;
        }
        else {
            buffer += " ";
            ++len;
        }
    }

    buffer += "\x1b[m";     // switch to normal formatting (white text; black background)
}