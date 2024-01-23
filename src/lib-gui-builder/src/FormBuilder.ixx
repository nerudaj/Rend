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
    addOption(const std::string& labelText, tgui::Widget::Ptr widget);

    void build();

private:
    tgui::Panel::Ptr panel;
    unsigned labelFontSize;
    std::vector<std::tuple<std::string, tgui::Widget::Ptr>> rowsToBuild;
};

module :private;

FormBuilder&
FormBuilder::addOption(const std::string& labelText, tgui::Widget::Ptr widget)
{
    rowsToBuild.push_back({ labelText, widget });
    return *this;
}

void FormBuilder::build()
{
    unsigned rowIdx = 0;

    auto paddedPanel = tgui::Panel::create({ "90%", "90%" });
    paddedPanel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    paddedPanel->setPosition({ "1.125%", "2%" });
    panel->add(paddedPanel);

    for (auto&& [labelText, widgetPtr] : rowsToBuild)
    {
        auto row = WidgetBuilder::createOptionRow(labelText, widgetPtr);
        row->setPosition("0%", row->getSize().y * rowIdx);
        paddedPanel->add(row);
        ++rowIdx;
    }
}
