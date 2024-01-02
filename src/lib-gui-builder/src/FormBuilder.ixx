module;

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <string>
#include <tuple>
#include <vector>

export module FormBuilder;

import WidgetBuilder;

export class [[nodiscard]] FormBuilder final
{
public:
    FormBuilder(tgui::Panel::Ptr panel) : panel(panel) {}

public:
    [[nodiscard]] FormBuilder&
    addOption(const std::string& labelText, tgui::Widget::Ptr widget)
    {
        rowsToBuild.push_back({ labelText, widget });
        return *this;
    }

    void build()
    {
        unsigned rowIdx = 0;

        for (auto&& [labelText, widgetPtr] : rowsToBuild)
        {
            auto row = WidgetBuilder::createOptionRow(labelText, widgetPtr);
            row->setPosition("0%", row->getSize().y * rowIdx);
            panel->add(row);
            ++rowIdx;
        }
    }

private:
    tgui::Panel::Ptr panel;
    unsigned labelFontSize;
    std::vector<std::tuple<std::string, tgui::Widget::Ptr>> rowsToBuild;
};
