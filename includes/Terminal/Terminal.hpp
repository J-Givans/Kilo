#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <termios.h>

class Terminal {
public:
    Terminal();
    ~Terminal();

    /// \brief Set the terminal driver into raw mode
    void enableRawMode();

private:
    enum class TerminalState { Reset, Raw };

    termios m_terminal;
    TerminalState m_state = TerminalState::Reset;
};

#endif