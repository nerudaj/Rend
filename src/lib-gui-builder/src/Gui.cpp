#include "Gui.hpp"
#include "Configs/Sizers.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Tgui.hpp>
#include <filesystem>
#include <ranges>

tgui::ChildWindow::Ptr Gui::createNewChildWindow(const std::string& title)
{
    auto modal = tgui::ChildWindow::create(title);
    modal->setRenderer(theme->getRenderer("ChildWindow"));
    modal->getRenderer()->setTitleBarHeight(
        static_cast<float>(Sizers::GetMenuBarHeight()));
    modal->setTitleTextSize(Sizers::GetMenuBarTextHeight());
    return modal;
}

bool Gui::isAnyModalOpened() const
{
    return !std::ranges::empty(
        gui.getWidgets()
        | std::views::filter(
            [](const tgui::Widget::Ptr& ptr)
            { return ptr->getWidgetType() == "ChildWindow"; }));
}
