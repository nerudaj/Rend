#include "ButtonListBuilder.hpp"
#include "WidgetBuilder.hpp"
#include <Configs/Sizers.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

ButtonListBuilder& ButtonListBuilder::addButton(
    const std::string& label, std::function<void(void)> onClick)
{
    buttonProps.emplace_back(label, onClick);
    return *this;
}

tgui::Panel::Ptr ButtonListBuilder::build()
{
    auto&& outerPanel = WidgetBuilder::createPanel();

    auto&& buttonListHeight =
        buttonProps.size() * Sizers::getBaseContainerHeight() * 2.1f;
    auto&& panel = WidgetBuilder::createPanel({ "60%", buttonListHeight });
    panel->setPosition(
        { "20%",
          ("parent.height - " + std::to_string(buttonListHeight)).c_str() });
    outerPanel->add(panel);

    for (auto&& [idx, props] : std::views::enumerate(buttonProps))
    {
        auto&& button = WidgetBuilder::createButton(props.label, props.onClick);
        button->setSize({ "100%", Sizers::getBaseContainerHeight() * 1.5f });
        button->setTextSize(Sizers::getBaseTextSize() * 2.f);
        button->setPosition(
            { "0%", Sizers::getBaseContainerHeight() * idx * 2.1f });
        panel->add(button);
    }

    return outerPanel;
}