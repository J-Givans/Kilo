#ifndef KEYS_HPP
#define KEYS_HPP

/// Arrow keys are represented using integers to avoid conflicts with the regular w a s d keys
enum Keys {
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

/// Bitwise-ANDs a char with the value 0x1f (or 0b00011111), thus setting the upper 3 bits of the character to 0
/// This mirrors what the CTRL key does in the terminal  
inline static constexpr char ctrlKey(char key)
{
    return key &= 0x1f;
}

#endif