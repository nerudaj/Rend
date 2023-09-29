#pragma once

#include <SFML/System/Vector2.hpp>

struct Position
{
    sf::Vector2f value;
};

struct Direction
{
    sf::Vector2f value;
};

[[nodiscard]] constexpr static float
operator""_px(unsigned long long value) noexcept
{
    return static_cast<float>(value);
}