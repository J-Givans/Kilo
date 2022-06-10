#include "Terminal.hpp"
#include "posix/lib.hpp"

#include <cstdlib>
#include <iostream>
#include <cctype>
#include <system_error>
#include <utility>

#include <stdexcept>
#include <unistd.h>

int main()
{
    Terminal kilo {};

    while (true) {
        char c {'\0'};

        try {
            if (auto [_, ec] = posix::read(STDIN_FILENO, &c, 1); ec && ec != std::errc::resource_unavailable_try_again) {
                throw std::system_error {ec, "read"};
            }
        }
        catch (std::system_error const& err) {
            std::cerr << "Caught: " << err.code() << ": " << err.code().message() << "\r\n";
            std::cerr << err.what() << "\r\n";
            
            std::exit(EXIT_FAILURE);
        }

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