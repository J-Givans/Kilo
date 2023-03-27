#include "Offset/Offset.hpp"
#include "Vec2/Vec2.hpp"

using Vector2i = Vector2<int>;

[[deprecated("Prefer direct access to the position member")]]
Vector2i const& Offset::getPosition() const& noexcept
{
    return position;
}

[[deprecated("Prefer direct access to the position member")]]
Vector2i& Offset::getPosition() & noexcept
{
    return position;
}

[[deprecated("Prefer direct access to the position member")]]
Vector2i Offset::getPosition() && noexcept
{
    return std::move(position);
}