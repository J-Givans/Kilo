#ifndef OFFSET_HPP
#define OFFSET_HPP

#include "Vec2/Vec2.hpp"

#include <utility>

struct Offset
{
    using Vector2i = Vector2<int>;

    constexpr Offset() noexcept = default;

    Vector2i const& getPosition() const& noexcept;

    Vector2i& getPosition() & noexcept;

    Vector2i getPosition() && noexcept;

    Vector2i m_position;
};

#endif
