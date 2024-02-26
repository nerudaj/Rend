#include <Configs/Sizers.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

import WidgetBuilder;
import TguiHelper;

[[nodiscard]] std::string randomString(std::size_t len)
{
    constexpr const char* CHARS =
        "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return std::views::iota(0u, len)
           | std::views::transform([CHARS](std::size_t)
                                   { return CHARS[rand() % std::size(CHARS)]; })
           | std::ranges::to<std::string>();
}

tgui::Label::Ptr WidgetBuilder::createLabelInternal(
    const std::string& text, const float sizeMultiplier, const bool justify)
{
    auto&& label = tgui::Label::create(text);
    label->getRenderer()->setTextColor(sf::Color::White);
    label->getRenderer()->setTextOutlineColor(tgui::Color::Black);
    label->getRenderer()->setTextOutlineThickness(1.f);
    label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    label->setHorizontalAlignment(
        justify ? tgui::Label::HorizontalAlignment::Center
                : tgui::Label::HorizontalAlignment::Left);
    label->setTextSize(Sizers::getBaseTextSize() * sizeMultiplier);
    label->setSize({ "100%", "100%" });
    return label;
}

tgui::Panel::Ptr
WidgetBuilder::createPanel(const tgui::Layout2d& size, const tgui::Color color)
{
    auto&& panel = tgui::Panel::create(size);
    panel->setPosition({ "0%", "0%" });
    panel->getRenderer()->setBackgroundColor(color);
    return panel;
}

tgui::Button::Ptr WidgetBuilder::createButton(
    const std::string& label, std::function<void(void)> onClick)
{
    auto&& button = tgui::Button::create(label);
    button->onClick(onClick);
    button->setTextSize(Sizers::getBaseTextSize());
    button->setSize({ "100%", "100%" });
    return button;
}

tgui::Panel::Ptr WidgetBuilder::createSlider(
    float value,
    std::function<void(float)> onChange,
    mem::Rc<tgui::Gui> gui,
    std::function<std::string(float)> valueFormatter,
    float lo,
    float hi,
    float step)
{
    auto&& result = tgui::Panel::create();
    result->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    const auto&& ID = randomString(16);

    auto&& dummyLabel = tgui::Label::create(valueFormatter(hi + step));
    dummyLabel->setTextSize(Sizers::GetMenuBarTextHeight());
    dummyLabel->setAutoSize(true);
    gui->add(dummyLabel, "DummyLabel");
    auto size = dummyLabel->getSizeLayout();

    auto&& valueLabel = tgui::Label::create(valueFormatter(value));
    valueLabel->setTextSize(Sizers::GetMenuBarTextHeight());
    valueLabel->getRenderer()->setTextColor(sf::Color::Black);
    valueLabel->setSize(size.x, "100%");
    valueLabel->setPosition("parent.width" - size.x, "0%");
    valueLabel->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    valueLabel->setHorizontalAlignment(
        tgui::Label::HorizontalAlignment::Center);
    result->add(valueLabel, ID);

    auto&& slider = tgui::Slider::create(lo, hi);
    slider->setPosition("2%", "25%");
    slider->setSize("parent.width - 4% - " - size.x, "50%");
    slider->setStep(step);
    slider->setValue(value);
    slider->onValueChange(
        [gui, ID, onChange, valueFormatter](float value)
        {
            gui->get<tgui::Label>(ID)->setText(valueFormatter(value));
            onChange(value);
        });
    result->add(slider);

    gui->remove(gui->get<tgui::Label>("DummyLabel"));

    return result;
}