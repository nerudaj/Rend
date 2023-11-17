#pragma once

#include "Gui.hpp"
#include "Interfaces/SidebarUserInterface.hpp"
#include "Tools/ToolPenHistory.hpp"

import Memory;

class SidebarUserWithSprites : public SidebarUserInterface
{
public:
    SidebarUserWithSprites(mem::Rc<Gui> gui) noexcept
        : SidebarUserInterface(gui)
    {
    }

public:
    [[nodiscard]] constexpr unsigned getPenValue() const noexcept
    {
        return penValue;
    }

protected:
    virtual void buildSidebarInternal(tgui::Group::Ptr& sidebar) override;
    void buildSpriteIdSelectionModal();
    void changePenValue(unsigned value);

    void selectSpriteId();

    [[nodiscard]] virtual tgui::Texture
    getSpriteAsTexture(unsigned spriteId) const = 0;

    [[nodiscard]] virtual std::size_t getSpriteCount() const = 0;

    [[nodiscard]] virtual bool shouldSkipSprite(unsigned) const
    {
        return false;
    }

protected:
    unsigned penValue = 0;
    ToolPenHistory penHistory;
};
