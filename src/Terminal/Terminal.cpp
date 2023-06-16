#include "Terminal/Terminal.hpp"

#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <system_error>

#include <fmt/core.h>
#include <gsl/assert>

namespace 
{
    /// \brief Make changes to the terminal driver such that it is set into non-canonical raw mode
    /// \param[inout] terminalHandle struct containing settings to be written to the terminal driver to enact this change
    void ttyRaw(termios& terminalHandle) noexcept;
    
    /// \brief Verify that the changes made to the terminal driver were successful
    /// \param[inout] terminalHandle struct containing the current settings of the terminal driver
    [[nodiscard]]
    bool verifyTtyRaw(termios const& terminalHandle) noexcept;
}

/// \details Query the terminal driver and write its settings to m_terminal
/// \details Exit the program if this fails
Terminal::Terminal()
{
    Expects(m_state == TerminalState::Reset);

    // Attempt to query the terminal driver and write its settings to m_terminal
    // If this fails, log the error and exit the program with status EXIT_FAILURE
    if (errno = 0; tcgetattr(STDIN_FILENO, &m_terminal) == -1) {
        fmt::print(stderr, "tcgetattr failed: {}", std::strerror(errno));
        std::exit(EXIT_FAILURE);
    }

    Ensures(m_state == TerminalState::Reset);
}

/// \details Reset the terminal driver to canonical mode
/// \details Exits the program if this fails
Terminal::~Terminal()
{
    Expects(m_state == TerminalState::Raw);

    // Attempt to set the terminal driver settings to those in m_terminal
    // If this fails, log the error that occured and exit the program with status EXIT_FAILURE
    if (errno = 0; tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_terminal) == -1) {
        fmt::print(stderr, "tcsetattr failed: {}", std::strerror(errno));
        std::exit(EXIT_FAILURE);
    }

    m_state = TerminalState::Reset;

    Ensures(m_state == TerminalState::Reset);
}

void Terminal::enableRawMode()
{
    Expects(m_state == TerminalState::Reset);

    // We expect to have populated m_terminal with the terminal driver settings
    // Copy these into `copy`, from which we'll make our changes
    termios copy {m_terminal};
    
    ttyRaw(copy);

    // Attempt to set the above settings to the terminal driver
    // If this fails, throw an exception
    if (errno = 0; tcsetattr(STDIN_FILENO, TCSAFLUSH, &copy) == -1) {
        throw std::system_error(errno, std::generic_category(), "Could not set the terminal driver to raw mode ");
    }

    // Verify that the changes stuck. tcsetattr can return 0 on partial success
    // First, attempt to query the terminal driver and store its settings in copy
    // If this fails, throw an exception
    
    if (errno = 0; tcgetattr(STDIN_FILENO, &copy) == -1) {
        throw std::system_error(errno, std::generic_category(), "Could not query the terminal driver to verify that it was set to raw mode ");
    }

    // We then check if the relevant fields match the changes we made
    if (verifyTtyRaw(copy)) {
        // Only some of the changes were made
        // Restore the original settings
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_terminal);
        throw std::system_error(EINVAL, std::generic_category(), "Setting terminal driver to raw mode partially succssful ");
    }

    m_state = TerminalState::Raw;

    Ensures(m_state == TerminalState::Raw);
}

namespace
{
    void ttyRaw(termios& terminalHandle) noexcept
    {
        // Ignore BREAK condition, no SIGINT on BREAK condition; don't mark parity condition; don't strip 8th bit from input
        // Don't map NL to CR on input; disable start/stop output flow ctrl
        terminalHandle.c_iflag &= ~(static_cast<unsigned>(BRKINT | ICRNL | INPCK | ISTRIP | IXON));
        
        // Disable all output processing
        terminalHandle.c_oflag &= ~(static_cast<unsigned>(OPOST));
        
        // Disable echoing (except for new lines), canonical mode, signal chars, and extended input processing 
        terminalHandle.c_lflag &= ~(static_cast<unsigned>(ECHO | ICANON | ISIG | IEXTEN));
        
        // Set 8 bits/char
        terminalHandle.c_cflag |= CS8;

        // Read with timeout
        // read() returns as soon as at least 1 byte is available, 
        terminalHandle.c_cc[VMIN] = 0;
        
        // or when TIME tenths of a second have elapsed
        terminalHandle.c_cc[VTIME] = 1;
    }

    [[nodiscard]]
    bool verifyTtyRaw(termios const& terminalHandle) noexcept
    {
        return (terminalHandle.c_iflag & (BRKINT | ICRNL | INPCK | ISTRIP | IXON)) 
            or (terminalHandle.c_oflag & OPOST) 
            or (terminalHandle.c_lflag & (ECHO | ICANON | ISIG | IEXTEN))
            or ((terminalHandle.c_cflag & CS8) != CS8)
            or (terminalHandle.c_cc[VMIN] != 0)
            or (terminalHandle.c_cc[VTIME] != 1);
    }
}