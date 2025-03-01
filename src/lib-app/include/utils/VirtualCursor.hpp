#pragma once

#include <DGM/dgm.hpp>
#include <PhysicalController.hpp>

class [[nodiscard]] VirtualCursor final
{
public:
    VirtualCursor(
        const sf::RenderWindow& window,
        const PhysicalController& input) noexcept
        : window(window), input(input) {};
    VirtualCursor(const VirtualCursor&) = delete;
    VirtualCursor(VirtualCursor&&) = delete;

public:
    void update(const float dt);

private:
    void clampCursorPosition(const sf::Vector2u& windowSize);

private:
    const sf::RenderWindow& window;
    const PhysicalController& input;
    sf::Vector2f position;
};