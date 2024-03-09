#pragma once

#include "Tools/SidebarUserWithSprites.hpp"
#include <DGM/classes/Clip.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <filesystem>

import TguiHelper;

class SidebarUserMesh : public SidebarUserWithSprites
{
public:
    SidebarUserMesh(mem::Rc<Gui> gui) noexcept : SidebarUserWithSprites(gui) {}

public:
    void configure(const std::filesystem::path& texturePath, dgm::Clip&& clip);

    [[nodiscard]] sf::Texture& getTexture() noexcept
    {
        return texture;
    }

    [[nodiscard]] const dgm::Clip& getClip() const noexcept
    {
        return clip;
    }

public:
    [[nodiscard]] tgui::Texture
    getSpriteAsTexture(unsigned spriteId) const override
    {
        return TguiHelper::convertTexture(texture, clip.getFrame(spriteId));
    }

    [[nodiscard]] std::size_t getSpriteCount() const override
    {
        return clip.getFrameCount();
    }

protected:
    sf::Texture texture;
    dgm::Clip clip;
};
