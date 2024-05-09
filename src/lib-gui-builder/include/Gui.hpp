#pragma once

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Tgui.hpp>
#include <memory>
#include <set>
#include <string>

class [[nodiscard]] Gui final
{
public:
    tgui::ChildWindow::Ptr createNewChildWindow(const std::string& title);

    [[nodiscard]] bool isAnyModalOpened() const;

    void closeModal(const std::string& id)
    {
        auto&& modal = get<tgui::ChildWindow>(id);
        gui.remove(modal);
    }

    template<class T>
    T::Ptr get(const std::string& id) const
    {
        return gui.get<T>(id);
    }

    void add(const tgui::Widget::Ptr& widget)
    {
        gui.add(widget);
    }

    void add(const tgui::Widget::Ptr& widget, const tgui::String& id)
    {
        gui.add(widget, id);
    }

    void draw()
    {
        gui.draw();
    }

    void removeAllWidgets()
    {
        gui.removeAllWidgets();
    }

public:
    tgui::Gui gui;
    std::shared_ptr<tgui::Theme> theme = tgui::Theme::create();
};