#pragma once

#include "Configs/Sizers.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Tgui.hpp>

// Gui adapter class
class [[nodiscard]] Gui final
{
public:
    Gui(tgui::Gui& gui) noexcept : gui(gui) {}

public:
    tgui::ChildWindow::Ptr createNewChildWindow(const std::string& title)
    {
        auto modal = tgui::ChildWindow::create(title);
        modal->setRenderer(theme.getRenderer("ChildWindow"));
        modal->getRenderer()->setTitleBarHeight(
            static_cast<float>(Sizers::GetMenuBarHeight()));
        modal->setTitleTextSize(Sizers::GetMenuBarTextHeight());
        return modal;
    }

    void addModal(const tgui::ChildWindow::Ptr& window, const std::string& id)
    {
        childWindowIds.insert(id);
        gui.add(window, id);
    }

    bool isAnyModalOpened() const
    {
        for (auto&& id : childWindowIds)
        {
            if (gui.get<tgui::ChildWindow>(id) != nullptr) return true;
        }

        return false;
    }

    void closeModal(const std::string& id)
    {
        auto modal = gui.get<tgui::ChildWindow>(id);
        gui.remove(modal);
    }

    template<class T>
    T::Ptr get(const std::string& id) const
    {
        return gui.get<T>(id);
    }

public:
    tgui::Gui& gui;
    tgui::Theme theme;

private:
    std::set<std::string> childWindowIds;
};
