#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <termios.h>

class Terminal {
public:
    Terminal();
    ~Terminal();

    /// \brief Set the terminal driver into raw mode
    void enableRawMode() const;

    /** Disable copy and move semantics */
    Terminal(Terminal const&) = delete;
    Terminal& operator=(Terminal const&) = delete;

private:
    termios m_terminal {};
};

#endif