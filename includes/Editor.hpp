#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "Keys.hpp"
#include "Terminal.hpp"
#include "posix/lib.hpp"

#include <cstddef>
#include <sstream>
#include <sys/ioctl.h>

#include <string>
#include <utility>
#include <filesystem>
#include <vector>

class Editor : public Terminal 
{
public:
    Editor() = default;
    ~Editor();

    static Editor& instance();
    int readKey();
    void processKeypress();
    void refreshScreen();
    void open(std::filesystem::path const& path);

private:
    /// Data type representing the position of the cursor in the terminal window
    struct Cursor
    {
        int xPos{};
        int yPos{};

        /// Move the cursor in the direction of the arrow key pressed
        void moveCursor(int const key, posix::winsize_t const& window)
        {
            auto [row, col] = window.getWindowSize();
    
            switch (key) {
            case ARROW_LEFT:
                if (xPos != 0) { --xPos; }
                break;
            
            case ARROW_RIGHT:
                if (xPos != col - 1) { ++xPos; }
                break;

            case ARROW_UP:
                if (yPos != 0) { --yPos; }
                break;

            case ARROW_DOWN:
                if (yPos != row - 1) { ++yPos; }
                break;
            }
        }
    };

    /// The position of the cursor in the terminal window
    Cursor m_cursor{};

    /// The version of this application
    std::string_view KILO_VERSION{ "0.0.1" };

    /// The size of the terminal window
    posix::winsize_t m_winsize{};

    /// The number of rows of text in the file
    int m_numRows{};

    /// Container to store the text read from the file passed during program initialisation
    std::vector<std::string> m_rowsOfText{};

    /// Keeps track of what row of the file the user is currently scrolled to
    /// We are scrolled to the top of the file by default
    int m_rowOffset{0};

    // Disable copy and move semantics
    Editor(Editor const&) = delete;
    Editor& operator=(Editor const&) = delete;

    void drawRows(std::stringstream& buffer);
};

#endif