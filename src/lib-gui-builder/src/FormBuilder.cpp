#include "FormBuilder.hpp"
#include "WidgetBuilder.hpp"
#include <Configs/Sizers.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

FormBuilder& FormBuilder::addOption(
    const std::string& labelText, tgui::Widget::Ptr widget, OptionConfig config)
{
    widget->setEnabled(!config.disabled);
    config.tooltipText.and_then(
        [&widget](const std::string& text)
        {
            widget->setToolTip(WidgetBuilder::createTextLabel(text));
            return std::optional(text);
        });
    rowsToBuild.push_back({ .label = labelText, .widget = widget });
    return *this;
}

FormBuilder& FormBuilder::addOptionWithWidgetId(
    const std::string& labelText,
    tgui::Widget::Ptr widget,
    const std::string widgetId)
{
    rowsToBuild.push_back(
        { .label = labelText, .widget = widget, .widgetId = widgetId });
    return *this;
}

FormBuilder& FormBuilder::addOptionWithSubmit(
    const std::string& labelText,
    tgui::Widget::Ptr widget,
    tgui::Button::Ptr submitBtn)
{
    rowsToBuild.push_back(
        { .label = labelText, .widget = widget, .submitBtn = submitBtn });
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

        auto&& row = props.submitBtn
                         ? WidgetBuilder::createOptionRowWithSubmitButton(
                             props.label, props.widget, props.submitBtn.value())
                         : WidgetBuilder::createOptionRow(
                             props.label, props.widget, props.widgetId);
        row->setPosition({ "0%", row->getSize().y * idx });
        panel->add(row);
    }

    return panel;
}