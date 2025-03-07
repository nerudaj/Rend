#pragma once

#include "Gui.hpp"
#include <DGM/dgm.hpp>
#include <PhysicalController.hpp>

class [[nodiscard]] VirtualCursor final
{
public:
    VirtualCursor(
        const sf::RenderWindow& window,
        PhysicalController& input,
        const dgm::ResourceManager& resmgr) noexcept
        : window(window), input(input)
    {
        sprite.setTexture(resmgr.get<sf::Texture>("cursor.png").value().get());
    };

    VirtualCursor(const VirtualCursor&) = delete;
    VirtualCursor(VirtualCursor&&) = delete;

public:
    void update(const float dt);

    void emulateClick(tgui::Gui& gui);

    void draw(dgm::Window& window);

private:
    void clampCursorPosition(const sf::Vector2u& windowSize);

private:
    const sf::RenderWindow& window;
    PhysicalController& input;
    sf::Vector2f position;
    sf::Sprite sprite;
};