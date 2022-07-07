#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <termios.h>

#include <utility>

class Terminal {
public:
    Terminal();
    ~Terminal();

    /** Disable copy and move semantics */
    Terminal(Terminal const&) = delete;
    Terminal& operator=(Terminal const&) = delete;

    void enableRawMode();
    void disableRawMode();

private:
    struct termios m_terminal {};
};

#endif