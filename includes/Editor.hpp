#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "Terminal.hpp"
#include "Keys.hpp"
#include "Cursor.hpp"

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
        
    static constexpr std::string_view KILO_VERSION {"0.0.1"};

    Cursor cursor {};
};


#endif