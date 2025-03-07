#include "utils/VirtualCursor.hpp"

void VirtualCursor::update(const float dt)
{
    input.update();
    const auto delta = input.getCursorDelta();
    if (delta == sf::Vector2f(0.f, 0.f)) return;

    position += delta * dt;

    clampCursorPosition(window.getSize());
    sf::Mouse::setPosition(sf::Vector2i(position), window);
}

void VirtualCursor::clampCursorPosition(const sf::Vector2u& windowSize)
{
    position.x = std::clamp(position.x, 0.f, static_cast<float>(windowSize.x));
    position.y = std::clamp(position.y, 0.f, static_cast<float>(windowSize.y));
}
