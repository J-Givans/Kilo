#include "Terminal/Terminal.hpp"

#include <gmock/gmock.h>

TEST(TerminalTest, WritesTerminalDriverSettingsToLocalBufferByDefault)
{
    Terminal terminal;
    ASSERT_THAT(terminal.getState(), testing::Eq(Terminal::TerminalState::Reset));
}