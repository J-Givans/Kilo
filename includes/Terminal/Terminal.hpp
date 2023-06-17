#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <termios.h>

class Terminal 
{
public:
    enum class TerminalState { Reset, Raw };

    Terminal();
    ~Terminal();

    /// \brief Set the terminal driver into raw mode
    void enableRawMode();

    /// \brief Get the current state of the terminal driver
    /// \brief For testing purposes only!
    TerminalState getState() const noexcept;

private:
    termios m_terminal;
    TerminalState m_state;
};

#endif