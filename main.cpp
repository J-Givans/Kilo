#include "Terminal.hpp"
#include "lib.hpp"

#include <iostream>
#include <cctype>

#include <unistd.h>

int main()
{
    Terminal term {};

    while (true) {
        char c {'\0'};
        posix::read(STDIN_FILENO, &c, 1);

        if (std::iscntrl(c)) {
            std::cout << static_cast<int>(c) << "\r\n";
        }
        else {
            std::cout << static_cast<int>(c) << " ('" << c << "')\r\n";
        }

        if (c == 'q') {
            break;
        }
    }

    return 0;
}