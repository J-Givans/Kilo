#include "Terminal.hpp"
#include "posix/lib.hpp"

#include <termios.h>
#include <unistd.h>

#include <system_error>
#include <iostream>

void Terminal::enableRawMode()
{
    std::error_code ec { posix::tcgetattr(STDIN_FILENO, m_terminal) };

    if (ec) {
        throw std::system_error {ec, "tcgetattr failed while enabling raw mode."};
    }

    struct termios copy {m_terminal};

    copy.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    copy.c_oflag &= ~(OPOST);
    copy.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    copy.c_cflag |= (CS8);
    copy.c_cc[VMIN] = 0;
    copy.c_cc[VTIME] = 1;

    ec = posix::tcsetattr(STDIN_FILENO, TCSAFLUSH, copy);

    if (ec) {
        throw std::system_error {ec, "tcsetattr failed while enabling raw mode."};
    }
}

void Terminal::disableRawMode()
{
    std::error_code ec {posix::tcsetattr(STDIN_FILENO, TCSAFLUSH, m_terminal)};

    if (ec) {
        throw std::system_error {ec, "tcsetattribute failed while disabling raw mode."};
    }
}

Terminal::Terminal()
{
    try {
        enableRawMode();
    }
    catch (std::system_error const& err) {
        std::cerr << err.what() << "\r\n";
        std::cerr << err.code() << ": " << err.code().message() << "\r\n";
    }
}

Terminal::~Terminal()
{
    try {
        disableRawMode();
    }
    catch (std::system_error const& err) {
        std::cerr << err.what() << "\r\n";
        std::cerr << err.code() << ": " << err.code().message() << "\r\n";
    }
}