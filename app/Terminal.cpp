#include "Terminal.hpp"
#include "posix/lib.hpp"

#include <termios.h>
#include <unistd.h>

#include <system_error>
#include <iostream>

void Terminal::enableRawMode()
{
    try {
        posix::tcgetattr(STDIN_FILENO, m_terminal);
    }
    catch (std::system_error const& err) {
        std::cerr << err.code().message() << ' ' << err.what() << '\n';
    }

    struct termios copy {m_terminal};

    copy.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    copy.c_oflag &= ~(OPOST);
    copy.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    copy.c_cflag |= (CS8);
    copy.c_cc[VMIN] = 0;
    copy.c_cc[VTIME] = 1;

    try {
        posix::tcsetattr(STDIN_FILENO, TCSAFLUSH, copy);
    }
    catch (std::system_error const& err) {
        std::cerr << err.code().message() << ' ' << err.what() << '\n';
    }
}

void Terminal::disableRawMode()
{
    try {
        posix::tcsetattr(STDIN_FILENO, TCSAFLUSH, m_terminal);
    }
    catch (std::system_error const& err) {
        std::cerr << err.code().message() << ' ' << err.what() << '\n';
    }
}

Terminal::Terminal()
{
    enableRawMode();
}

Terminal::~Terminal()
{
    disableRawMode();
}