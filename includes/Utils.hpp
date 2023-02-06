#ifndef UTILS_HPP
#define UTILS_HPP

#include <posix/lib.hpp>

/**
 * @brief Perform low-level keypress handling
 * 
 * @return int The character input by the user
 */
[[nodiscard]]
int readKey();

#endif