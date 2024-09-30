#pragma once

#include "Gui.hpp"
#include "Tools/SidebarUserWithSprites.hpp"
#include <Memory.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <enums/LevelItemId.hpp>
#include <enums/SpawnRotation.hpp>
#include <filesystem>
#include <utility>
#include <vector>

class [[nodiscard]] SidebarUserItem final : public SidebarUserWithSprites
{
public:
    struct [[nodiscard]] ItemRenderData final
    {
        sf::Texture texture;
        tgui::Texture guiTexture;
        sf::IntRect clip;
        sf::Sprite sprite;
    };

    struct [[nodiscard]] PathRectPair final
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
        SpawnRotation rotation,
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

    bool shouldSkipSprite(unsigned spriteId) const override
    {
        switch (static_cast<LevelItemId>(spriteId))
        {
            using enum LevelItemId;
        case Unused1:
        case Unused2:
        case Unused3:
        case UnusedFlag:
        case Unused4:
        case Unused5:
        case Unused6:
        case Unused7:
        case Unused8:
        case Unused9:
        case Unused10:
        case Unused11:
        case Unused12:
        case Unused13:
        case Unused14:
        case Unused15:
        case Unused16:
        case Unused17:
        case Unused18:
        case Unused19:
        case Unused20:
        case Unused21:
            return true;
        }

        return false;
    }

private:
    std::vector<ItemRenderData> renderData;
    sf::RectangleShape outline;
};
