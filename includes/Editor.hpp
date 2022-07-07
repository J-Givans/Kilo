#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "Terminal.hpp"
#include "Keys.hpp"
#include "Cursor.hpp"
#include "Window.hpp"

#include <cstddef>
#include <sys/ioctl.h>

#include <utility>
#include <string>

class Editor : public Terminal {
public:
    Editor() = default;
    ~Editor();
    
    /// Disable copy and move semantics
    Editor(Editor const&) = delete;
    Editor& operator=(Editor const&) = delete;

    /// Perform low-level keypress reading
    static int readKey();

    /// Map keypresses to editor operations
    void processKeypress();

    void refreshScreen() const;

    void open();

private:
    void drawRows(std::string& buffer) const;

private:
    static constexpr std::string_view KILO_VERSION {"0.0.1"};

    Cursor m_cursor {};
    Window m_window {};

    std::size_t m_numRows {};
    std::string m_rowOfText;
};


#endif