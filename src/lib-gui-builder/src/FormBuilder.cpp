#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

import FormBuilder;
import WidgetBuilder;

FormBuilder& FormBuilder::addOption(
    const std::string& labelText, tgui::Widget::Ptr widget, bool disabled)
{
    widget->setEnabled(!disabled);
    rowsToBuild.push_back({ labelText, widget });
    return *this;
}

void FormBuilder::build()
{
    unsigned rowIdx = 0;

    auto paddedPanel = tgui::Panel::create({ "97.75%", "96%" });
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