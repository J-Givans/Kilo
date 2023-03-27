#ifndef OFFSET_HPP
#define OFFSET_HPP

#include "Vec2/Vec2.hpp"

#include <utility>

struct Offset
{
    using Vector2i = Vector2<int>;

    constexpr Offset() noexcept = default;

    [[deprecated("Prefer direct access to the position member")]]
    Vector2i const& getPosition() const& noexcept;

    [[deprecated("Prefer direct access to the position member")]]
    Vector2i& getPosition() & noexcept;

    [[deprecated("Prefer direct access to the position member")]]
    Vector2i getPosition() && noexcept;

    Vector2i position;
};

#endif
