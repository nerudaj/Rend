#include "utils/VirtualCursor.hpp"

void VirtualCursor::update(const float dt)
{
    input.update();
    const auto delta = input.getCursorDelta();
    if (delta == sf::Vector2f(0.f, 0.f))
    {
        position = sf::Vector2f(sf::Mouse::getPosition(window));
        return;
    };

    position += delta * dt;

    clampCursorPosition(window.getSize());
    sf::Mouse::setPosition(sf::Vector2i(position), window);
}

void VirtualCursor::emulateClick(tgui::Gui& gui)
{
    gui.handleEvent(sf::Event { .type = sf::Event::MouseButtonPressed,
                                .mouseButton = {
                                    .button = sf::Mouse::Left,
                                    .x = static_cast<int>(position.x),
                                    .y = static_cast<int>(position.y),
                                } });
    gui.handleEvent(sf::Event { .type = sf::Event::MouseButtonReleased,
                                .mouseButton = {
                                    .button = sf::Mouse::Left,
                                    .x = static_cast<int>(position.x),
                                    .y = static_cast<int>(position.y),
                                } });
}

void VirtualCursor::draw(dgm::Window& window)
{
    sprite.setPosition(position);
    window.draw(sprite);
}

void VirtualCursor::clampCursorPosition(const sf::Vector2u& windowSize)
{
    position.x = std::clamp(position.x, 0.f, static_cast<float>(windowSize.x));
    position.y = std::clamp(position.y, 0.f, static_cast<float>(windowSize.y));
}
