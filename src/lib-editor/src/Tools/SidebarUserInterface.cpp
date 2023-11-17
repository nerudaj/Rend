#include "Interfaces/SidebarUserInterface.hpp"

void SidebarUserInterface::buildSidebar()
{
    auto sidebar = gui->get<tgui::Group>("Sidebar");
    sidebar->removeAllWidgets();

    // Back panel of the sidebar
    auto sidebgr = tgui::Panel::create({ "100%", "100%" });
    sidebgr->setRenderer(gui->theme.getRenderer("Panel"));
    sidebar->add(sidebgr);

    // The rest is up to actual tool
    buildSidebarInternal(sidebar);
}
