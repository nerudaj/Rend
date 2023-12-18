#pragma once

#include "DialogBase.hpp"
#include <DGM/dgm.hpp>
#include <LevelTheme.hpp>
#include <optional>

import Memory;

class [[nodiscard]] ModernNewLevelDialog final : public ModernDialogInterface
{
public:
    ModernNewLevelDialog(mem::Rc<Gui> gui);

public:
    [[nodiscard]] constexpr unsigned getLevelWidth() const noexcept
    {
        return width;
    }

    [[nodiscard]] constexpr unsigned getLevelHeight() const noexcept
    {
        return height;
    }

    [[nodiscard]] constexpr SkyboxTheme getSkyboxTheme() const noexcept
    {
        return skyboxTheme;
    }

    [[nodiscard]] constexpr TexturePack getTexturePack() const noexcept
    {
        return texturePack;
    }

    [[nodiscard]] constexpr std::string getAuthorName() const noexcept
    {
        return author;
    }

protected:
    void buildLayoutImpl(tgui::Panel::Ptr panel) override;

protected:
    const std::vector<std::string> ALLOWED_LEVEL_SIZES = { "16", "24", "32" };
    unsigned width = 16;
    unsigned height = 16;
    SkyboxTheme skyboxTheme = SkyboxTheme::Countryside;
    TexturePack texturePack = TexturePack::AlphaVersion;
    std::string author = "none";
};
