#pragma once

#include "Gui.hpp"
#include "Tools/SidebarUserWithSprites.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <filesystem>
#include <utility>
#include <vector>

import Memory;

class SidebarUserItem : public SidebarUserWithSprites
{
public:
    struct ItemRenderData
    {
        sf::Texture texture;
        tgui::Texture guiTexture;
        sf::IntRect clip;
        sf::Sprite sprite;
    };

    struct PathRectPair
    {
        std::filesystem::path texturePath;
        sf::IntRect clip;
    };

public:
    [[nodiscard]] SidebarUserItem(mem::Rc<Gui> gui) noexcept
        : SidebarUserWithSprites(gui)
    {
        outline.setOutlineColor(sf::Color::Red);
        outline.setOutlineThickness(2);
        outline.setFillColor(sf::Color::Transparent);
    }

public:
    void configure(const std::vector<PathRectPair>& textureClipPairs);

    [[nodiscard]] const sf::IntRect& getClip(unsigned spriteId) const noexcept
    {
        return renderData.at(spriteId).clip;
    }

    void drawSprite(
        tgui::CanvasSFML::Ptr& canvas,
        unsigned spriteId,
        const sf::Vector2i& position,
        bool selected,
        float opacity);

public: // SidebarUserWithSprites
    [[nodiscard]] tgui::Texture
    getSpriteAsTexture(unsigned spriteId) const override
    {
        return renderData[spriteId].guiTexture;
    }

    [[nodiscard]] std::size_t getSpriteCount() const override
    {
        return renderData.size();
    }

private:
    std::vector<ItemRenderData> renderData;
    sf::RectangleShape outline;
};
