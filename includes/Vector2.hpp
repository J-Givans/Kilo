#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <type_traits>

template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
struct Vector2
{
    T x {};
    T y {};
};

#endif
