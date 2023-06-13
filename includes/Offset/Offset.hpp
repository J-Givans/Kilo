#ifndef OFFSET_HPP
#define OFFSET_HPP

#include "Vec2/Vec2.hpp"

#include <utility>

struct Offset
{
    Vector2<int> position;
    
    constexpr Offset() noexcept = default;
};

#endif
