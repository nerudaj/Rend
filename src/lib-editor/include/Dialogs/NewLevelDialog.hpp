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

    [[nodisard]] constexpr LevelTheme getLevelTheme() const noexcept
    {
        return theme;
    }

protected:
    void buildLayoutImpl(tgui::Panel::Ptr panel) override;

protected:
    const std::vector<std::string> ALLOWED_LEVEL_SIZES = { "16", "24", "32" };
    unsigned width = 16;
    unsigned height = 16;
    LevelTheme theme = LevelTheme::Countryside;
};