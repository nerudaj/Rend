#include <Configs/Sizers.hpp>
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

tgui::Panel::Ptr FormBuilder::build(tgui::Color backgroundColor)
{
    auto&& panel = WidgetBuilder::createPanel(
        { "100%",
          Sizers::getBaseContainerHeight() * rowsToBuild.size() + 20.f },
        backgroundColor);
    panel->getRenderer()->setPadding({ 10.f, 10.f });

    for (auto&& [idx, props] : std::views::enumerate(rowsToBuild))
    {
        auto&& row = WidgetBuilder::createOptionRow(props.label, props.widget);
        row->setPosition({ "0%", row->getSize().y * idx });
        panel->add(row);
    }

    return panel;
}