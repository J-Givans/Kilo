#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "Terminal.hpp"

#include <sys/ioctl.h>

#include <utility>
#include <string>

class Editor : public Terminal {
public:
    Editor();
    ~Editor();
    
    /// Disable copy and move semantics
    Editor(Editor const&) = delete;
    Editor& operator=(Editor const&) = delete;

    /// Perform low-level keypress reading
    static int readKey();

    /// Map keypresses to editor operations
    void processKeypress();

    void refreshScreen() const;

    void init();

private:
    void drawRows(std::string& buffer) const;

    /// Move the cursor when a key is pressed
    void moveCursor(int const& key);

private:
    std::pair<unsigned short, unsigned short> m_windowDimensions;
    
    /// Horizontal coordinate (column) of the cursor
    int cursorX {};

    /// Vertical coordinate (row) of the cursor
    int cursorY {};
    
    static constexpr std::string_view KILO_VERSION {"0.0.1"};

    enum Keys { 
        // Arrow keys are represented using ints so that they don't conflict with regular w a s d keypresses
        ARROW_LEFT = 1000,
        ARROW_RIGHT,
        ARROW_UP,
        ARROW_DOWN,
        DELETE,
        HOME,
        END,
        PAGE_UP,
        PAGE_DOWN
    };
};

/// Bitwise-ANDs a char with the value 0x1f (or 0b00011111), thus setting the upper 3 bits of the character to 0
/// This mirrors what the CTRL key does in the terminal  
inline static constexpr char ctrlKey(char key)
{
    return key &= 0x1f;
}

#endif