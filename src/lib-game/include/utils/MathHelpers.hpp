#pragma once

#include <SFML/System/Vector2.hpp>
#include <bitset>
#include <cassert>
#include <functional>

[[nodiscard]] static inline sf::Vector2f
getPerpendicular(const sf::Vector2f& v) noexcept
{
    return { -v.y, v.x };
}

static inline void forEachDirection(
    const sf::Vector2f& direction,
    const sf::Vector2f& plane,
    unsigned DIRECTION_COUNT,
    std::function<void(const sf::Vector2f&)> callback)
{
    for (unsigned i = 0; i < DIRECTION_COUNT; i++)
    {
        float x = 2.f * i / DIRECTION_COUNT - 1.f;
        callback(direction + plane * x);
    }
}

template<std::size_t Bits>
[[nodiscard]] static constexpr std::size_t
getPrevToggledBit(std::size_t index, const std::bitset<Bits>& bitset) noexcept
{
    assert(bitset[0]);
    if (index == 0) index = bitset.size();
    do
    {
        --index;
    } while (!bitset[index]);
    return index;
}

template<std::size_t Bits>
[[nodiscard]] static constexpr std::size_t
getNextToggledBit(std::size_t index, const std::bitset<Bits>& bitset) noexcept
{
    assert(bitset[0]);
    do
    {
        ++index;
    } while (index < bitset.size() && !bitset[index]);
    return index == bitset.size() ? 0 : index;
}

// Returns number (either -1.f, 0.f or 1.f) based on which
// direction to rotate 'from' vector to get to 'to'
[[nodiscard]] constexpr float
getVectorPivotDirection(const sf::Vector2f& from, const sf::Vector2f& to)
{
    const float m = -to.y * from.x + to.x * from.y;
    return m == 0.f ? 0.f : -m / std::abs(m);
}
