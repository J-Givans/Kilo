#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "Keys.hpp"
#include "Terminal.hpp"
#include "posix/lib.hpp"

#include <string>
#include <filesystem>
#include <vector>

/// Keeps track of the cursor position the user is currently scrolled to
/// Default is (0,0) - the top of the file
struct Offset
{
    int row{};
    int col{};
};

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
        void moveCursor(int const key);
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
    
    Offset m_offset{};

    // Disable copy and move semantics
    Editor(Editor const&) = delete;
    Editor& operator=(Editor const&) = delete;

    void drawRows(std::stringstream& buffer);
    void scroll();
};

#endif