#include "Offset.hpp"
#include "Vector2.hpp"

using Vector2i = Vector2<int>;

Vector2i const& Offset::getPosition() const& noexcept
{
    return m_position;
}

Vector2i& Offset::getPosition() & noexcept
{
    return m_position;
}

Vector2i Offset::getPosition() && noexcept
{
    return std::move(m_position);
}