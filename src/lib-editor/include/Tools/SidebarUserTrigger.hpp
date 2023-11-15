#pragma once

#include "Interfaces/SidebarUserInterface.hpp"
#include <LevelD.hpp>

class SidebarUserTrigger final : public SidebarUserInterface
{
public:
    using PenType = LevelD::Trigger::AreaType;

public:
    [[nodiscard]] SidebarUserTrigger(mem::Rc<Gui> gui) noexcept
        : SidebarUserInterface(gui)
    {
    }

public:
    [[nodiscard]] constexpr PenType getPenType() const noexcept
    {
        return penType;
    }

protected: // SidebarUserInterface
    void buildSidebarInternal(tgui::Group::Ptr& sidebar) override;

protected:
    void changePen(PenType newPenType)
    {
        penType = newPenType;
        buildSidebar(); // to properly highlight active button
    }

protected:
    PenType penType = PenType::Circle;
};