#include "Editor.hpp"
#include "posix/lib.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include <string>
#include <iostream>

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

Editor& Editor::instance()
{
    static Editor editor{};
    return editor;
}

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

void Editor::processKeypress()
{
    auto [rows, cols] = m_winsize.getWindowSize();

    int c = readKey();

    switch (c) {
    case ctrlKey('q'):
        std::exit(EXIT_SUCCESS);
        break;

    case HOME:
        m_cursor.x = 0;
        break;

    case END:
        m_cursor.x = cols - 1;
        break;

    case PAGE_UP:
    case PAGE_DOWN: {
        auto iterations { rows };

        while (--iterations) {
            m_cursor.moveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN, m_winsize);
        }
    } break;

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
        m_cursor.moveCursor(c, m_winsize);
        break;
    }
}

void Editor::refreshScreen()
{
    std::stringstream buffer{};

    buffer << "\x1b[?25l"; // hide the cursor while repainting
    buffer << "\x1b[H"; // reposition the cursor to the top-left corner

    drawRows(buffer); // draw column of tildes

    buffer << "\x1b[" << m_cursor.y + 1 << ";" << m_cursor.x + 1 << "H";  // move the cursor to position (y+1, x+1)
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
            try {
                if (std::ssize(m_rowsOfText.at(fileRow)) > columns) {
                    m_rowsOfText.at(fileRow).resize(columns);
                }
            }
            catch (std::out_of_range const& err) {
                std::cerr << "Error while printing to screen\n";
                std::exit(EXIT_FAILURE);
            }
            
            buffer << m_rowsOfText.at(fileRow);
        }

        buffer << "\x1b[K"; // clear lines one at a time

        if (y < rows - 1) {
            buffer << "\r\n";
        }
    }
}

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