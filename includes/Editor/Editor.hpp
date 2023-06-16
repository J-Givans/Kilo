#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "Keys/Keys.hpp"
#include "Terminal/Terminal.hpp"
#include "Offset/Offset.hpp"
#include <winsize/winsize.hpp>

#include <string>
#include <filesystem>
#include <string_view>
#include <vector>

/// Data type representing the position of the cursor in the terminal window
struct Cursor
{
    int xPos{};
    int yPos{};

    /// Move the cursor in the direction of the arrow key pressed
    void moveCursor(Key const& key);
};

class Editor
{
    friend struct Cursor;

public:
    Editor();
    ~Editor();
    Editor(Editor const&) = delete;
    Editor& operator=(Editor const&) = delete;

    static Editor& instance();
    void processKeypress();
    void refreshScreen();
    void open(std::filesystem::path const& path);

private:
    Terminal m_terminalCtrl;
    Cursor m_cursor {};    /// The position of the cursor in the terminal window
    std::string_view KILO_VERSION{ "0.0.1" };   /// The version of this application
    kilo::lib::winsize::winsize m_winsize;  // The size of the terminal window
    int m_numRows {0};  /// The number of rows of text in the file
    std::vector<std::string> m_text;    /// Text read from the file opened
    Offset m_offset;
    std::string_view m_filename;     /// The name of the file currently opened by the editor

    void drawRows(std::string& buffer);
    void displayWelcomeMessage(std::string& buffer) const;
    void scroll();
    void drawStatusBar(std::string& buffer) const;
    void processKeypressHelper(Key const& key) noexcept;
};

#endif