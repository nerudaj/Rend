#pragma once

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

class [[nodiscard]] GuiOptionsBuilder2 final
{
public:
    GuiOptionsBuilder2(tgui::Panel::Ptr panel) : panel(panel) {}

public:
    [[nodiscard]] GuiOptionsBuilder2&
    addOption(const std::string& labelText, tgui::Widget::Ptr widget)
    {
        rowsToBuild.push_back({ labelText, widget });
        return *this;
    }

    void build();

private:
    tgui::Panel::Ptr panel;
    unsigned labelFontSize;
    std::vector<std::tuple<std::string, tgui::Widget::Ptr>> rowsToBuild;
};
