#pragma once

#include "include/Gui.hpp"
#include "include/Utilities/GC.hpp"

class SidebarUserInterface
{
public:
    SidebarUserInterface(GC<Gui> gui) noexcept : gui(gui) {}

    virtual ~SidebarUserInterface() = default;

public:
    void buildSidebar();

protected:
    virtual void buildSidebarInternal(tgui::Group::Ptr& sidebar) = 0;

protected:
    GC<Gui> gui;
};
