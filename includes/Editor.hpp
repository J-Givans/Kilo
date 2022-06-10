#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "Terminal.hpp"

class Editor : public Terminal {
public:
    /** Perform low-level keypress reading */
    static char readKey();

    /** Map keypresses to editor operations */
    static void processKeypress();

    static void refreshScreen();

private:
    /**
     * Bitwise-ANDs a char with the value 0x1f (or 0b00011111), thus setting the upper 3 bits of the character to 0
     * This mirrors what the CTRL key does in the terminal
    */
    static constexpr char ctrlKey(char key)
    {
        return key &= 0x1f;
    }
};

#endif