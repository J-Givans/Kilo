#ifndef KEYS_HPP
#define KEYS_HPP

#include <cstddef>
#include <bit>

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
    PAGE_DOWN,
    Escape
};

/**
 * @brief We use integer constants to represent the keys in order to avoid conflicts with the regular [w, a, s, d] keys
 */
enum class Key : int {
    ArrowLeft = 1000, ArrowRight, ArrowUp, ArrowDown,
    Delete,
    Home, End,
    PageUp, PageDown,
    Escape
};

/// Bitwise-ANDs a char with the value 0x1f (or 0b00011111), thus setting the upper 3 bits of the character to 0
/// This mirrors what the CTRL key does in the terminal  
constexpr unsigned char ctrlKey(char key) noexcept
{
    std::byte byte = std::bit_cast<std::byte>(key);
    byte &= std::byte { 0x1f };

    return std::to_integer<unsigned char>(byte);
}

[[nodiscard]]
constexpr bool isEscapeKey(unsigned char key) noexcept
{
    return key == '\x1b';
}

[[nodiscard]]
constexpr bool isEscapeKey(Key const& key) noexcept
{
    return key == Key::Escape;
}

[[nodiscard]]
constexpr bool isArrowKey(Key const& key) noexcept
{
    return (key == Key::ArrowUp) or (key == Key::ArrowDown) or (key == Key::ArrowLeft) or (key == Key::ArrowRight);
}

[[nodiscard]]
constexpr bool isPageKey(Key const& key) noexcept
{
    return (key == Key::PageUp) or (key == Key::PageDown);
}

[[nodiscard]]
constexpr bool isHomeKey(Key const& key) noexcept
{
    return key == Key::Home;
}

[[nodiscard]]
constexpr bool isEndKey(Key const& key) noexcept
{
    return key == Key::End;
}

[[nodiscard]]
constexpr bool isDeleteKey(Key const& key) noexcept
{
    return key == Key::Delete;
}

#endif