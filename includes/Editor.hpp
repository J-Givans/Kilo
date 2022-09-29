#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "Cursor.hpp"
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

    /// Perform low-level keypress reading
    int readKey();

    /// Map keypresses to editor operations
    void processKeypress();

    /// Print text to the screen
    void refreshScreen();

    void open(std::filesystem::path const& path);

private:
    /// The version of this application
    std::string_view KILO_VERSION{ "0.0.1" };

    /// The position of the cursor in the window
    Cursor m_cursor{};

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

    /// Draw a column of tildes on the left-hand side of the screen
    void drawRows(std::stringstream& buffer);
};

#endif