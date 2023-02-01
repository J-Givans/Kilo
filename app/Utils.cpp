#include "Utils.hpp"
#include "Keys.hpp"
#include "posix/lib.hpp"

#include <array>

/**
 * @brief Perform low-level keypress handling
 * @returns An integer representing the character that was input
 * 
*/
[[nodiscard]]
int readKey()
{
    [[maybe_unused]] std::size_t bytesRead = 0;
    unsigned char keyRead;

    while ((bytesRead = posix::read(STDIN_FILENO, &keyRead, 1)) != 1) {
        // Recall: from Terminal.cpp, VMIN = 0, VTIME = 1;
        // read returns [1, count] bytes before the timer expires, or 0 if the timer expires
    }

    /**
     * If we read an escape character, immediately read 2 more bytes into @c sequence.
     * If either of these reads times out, assume the user pressed ESC and return that instead.
     * Otherwise, look to see if the escape sequence if an arrow key sequence.
     * If it is, return the corresponding [w, a, s, d] character, else return ESC.
    */

    if (not isEscapeKey(keyRead)) {
        return static_cast<int>(keyRead);
    }

    std::array<unsigned char, 3> sequence;

    if (posix::read(STDIN_FILENO, &sequence[0], 1) != 1) {
        return static_cast<int>(Key::Escape);
    }
    else if (posix::read(STDIN_FILENO, &sequence[1], 1) != 1) {
        return static_cast<int>(Key::Escape);
    }

    if (sequence[0] == '[') {
        if (sequence[1] >= '0' and sequence[1] <= '9') {
            if (posix::read(STDIN_FILENO, &sequence[2], 1) != 1) {
                return static_cast<int>(Key::Escape);
            }

            if (sequence[2] == '~') {
                switch (sequence[1]) {
                    case '1': 
                    case '7':
                        return static_cast<int>(Key::Home);

                    case '3':
                        return static_cast<int>(Key::Delete);
                    
                    case '4':
                    case '8':
                        return static_cast<int>(Key::End);
                    
                    case '5':
                        return static_cast<int>(Key::PageUp);
                    
                    case '6':
                        return static_cast<int>(Key::PageDown);
                    
                    default:
                        break;                   
                }
            }
        }
        else {
            switch (sequence[1]) {
                case 'A':
                    return static_cast<int>(Key::ArrowUp);
                
                case 'B':
                    return static_cast<int>(Key::ArrowDown);
                
                case 'C':
                    return static_cast<int>(Key::ArrowRight);
                
                case 'D':
                    return static_cast<int>(Key::ArrowLeft);
                
                case 'H':
                    return static_cast<int>(Key::Home);
                
                case 'F':
                    return static_cast<int>(Key::End);
                
                default:
                    break;
            }
        }
    }
    else if (sequence[0] == 'O') {
        switch (sequence[1]) {
            case 'H':
                return static_cast<int>(Key::Home);
                
            case 'F':
                return static_cast<int>(Key::End);

            default:
                break;
        }
    }

    return static_cast<int>(Key::Escape);
}