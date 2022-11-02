#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "Keys.hpp"
#include "Terminal.hpp"
#include "posix/lib.hpp"

#include <sstream>
#include <string>
#include <filesystem>
#include <string_view>
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
    Editor();

    static Editor& instance();
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

    Cursor m_cursor;    /// The position of the cursor in the terminal window
    std::string_view KILO_VERSION{ "0.0.1" };   /// The version of this application
    posix::winsize_t m_winsize; /// The size of the terminal window
    int m_numRows;  /// The number of rows of text in the file
    std::vector<std::string> m_text;    /// Text read from the file opened
    Offset m_offset;
    std::string_view m_filename;     /// The name of the file currently opened by the editor

private:
    ~Editor();

    // Disable copy and move semantics
    Editor(Editor const&) = delete;
    Editor& operator=(Editor const&) = delete;

    int readKey();
    void drawRows(std::stringstream& buffer);
    void scroll();
    void drawStatusBar(std::stringstream& buffer);
};

#endif