#pragma once

#include "Gui.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Tgui.hpp>

import Memory;

class SidebarUserInterface
{
public:
    SidebarUserInterface(mem::Rc<Gui> gui) noexcept : gui(gui) {}

    virtual ~SidebarUserInterface() = default;

public:
    void buildSidebar();

protected:
    virtual void buildSidebarInternal(tgui::Group::Ptr& sidebar) = 0;

protected:
    mem::Rc<Gui> gui;
};
