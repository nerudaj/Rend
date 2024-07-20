#pragma once

#include "DialogInterface.hpp"
#include "MetadataDialogBase.hpp"
#include <LevelTheme.hpp>
#include <Memory.hpp>
#include <string>
#include <vector>

class [[nodiscard]] ModernNewLevelDialog final
    : public DialogInterface
    , public MetadataDialogBase
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

protected:
    void buildLayoutImpl(tgui::Panel::Ptr panel) override;

protected:
    const std::vector<std::string> ALLOWED_LEVEL_SIZES = { "16", "24", "32" };
    unsigned width = 16;
    unsigned height = 16;
};
