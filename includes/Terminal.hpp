#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <termios.h>

class Terminal {
public:
    Terminal();
    ~Terminal();

    Terminal(Terminal const&) = delete;
    Terminal& operator=(Terminal const&) = delete;

    void enableRawMode();
    void disableRawMode();

private:
    struct termios m_terminal;
};

#endif