#pragma once

#include "include/Tools/SidebarUserWithSprites.hpp"
#include <DGM/classes/Clip.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <filesystem>

class SidebarUserMesh : public SidebarUserWithSprites
{
public:
    SidebarUserMesh(GC<Gui> gui) noexcept : SidebarUserWithSprites(gui) {}

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
    // FIXME: Texture provider as a dependency
    [[nodiscard]] tgui::Texture
    getSpriteAsTexture(unsigned spriteId) const override
    {
        return tgui::Texture(texture, clip.getFrame(spriteId));
    }

    [[nodiscard]] std::size_t getSpriteCount() const override
    {
        return clip.getFrameCount();
    }

protected:
    sf::Texture texture;
    dgm::Clip clip;
};
