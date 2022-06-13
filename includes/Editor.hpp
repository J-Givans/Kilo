#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "Terminal.hpp"

#include <sys/ioctl.h>
#include <utility>

class Editor : public Terminal {
public:
    Editor() = default;
    ~Editor();
    
    /** Disable copy and move semantics */
    Editor(Editor const&) = delete;
    Editor& operator=(Editor const&) = delete;

    /** Perform low-level keypress reading */
    static char readKey();

    /** Map keypresses to editor operations */
    static void processKeypress();

    void refreshScreen() const;

    static void init(Editor const& editor);

private:
    /**
     * Bitwise-ANDs a char with the value 0x1f (or 0b00011111), thus setting the upper 3 bits of the character to 0
     * This mirrors what the CTRL key does in the terminal
    */
    static constexpr char ctrlKey(char key)
    {
        return key &= 0x1f;
    }

    void drawRows() const;

    std::pair<unsigned short, unsigned short> getWindowSize() const;

private:
    struct winsize m_window {};
};

#endif