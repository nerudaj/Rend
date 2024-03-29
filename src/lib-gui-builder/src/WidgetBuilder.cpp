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

tgui::Panel::Ptr WidgetBuilder::createOptionRow(
    const std::string& labelText, tgui::Widget::Ptr widgetPtr)
{
    auto&& row = getStandardizedRow();

    auto&& label = tgui::Label::create(labelText);
    label->getRenderer()->setTextColor(sf::Color::Black);
    label->setSize("60%", "100%");
    label->setPosition("0%", "0%");
    label->setTextSize(Sizers::GetMenuBarTextHeight());
    label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    row->add(label);

    auto&& widgetPanel = tgui::Panel::create({ "40%", "100%" });
    widgetPanel->setPosition("60%", "0%");
    widgetPanel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    row->add(widgetPanel);
    widgetPanel->add(widgetPtr);

    return row;
}

tgui::Panel::Ptr WidgetBuilder::getStandardizedRow(tgui::Color bgcolor)
{
    auto&& row = tgui::Panel::create();
    row->setSize("100%", std::to_string(Sizers::GetMenuBarHeight()).c_str());
    row->getRenderer()->setBackgroundColor(bgcolor);
    return row;
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

tgui::CheckBox::Ptr
WidgetBuilder::createCheckbox(bool checked, std::function<void(bool)> onChange)
{
    auto&& checkbox = tgui::CheckBox::create();
    auto&& size = checkbox->getSizeLayout();
    checkbox->setPosition(("100%" - size.x) / 2, ("100%" - size.y) / 2);
    checkbox->setChecked(checked);
    checkbox->onChange(onChange);
    return checkbox;
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

tgui::ComboBox::Ptr WidgetBuilder::createDropdown(
    const std::vector<std::string>& items,
    const std::string& selected,
    std::function<void(std::size_t)> onSelect)
{
    auto&& dropdown = tgui::ComboBox::create();
    dropdown->setSize("100%", "80%");
    dropdown->setPosition("0%", "10%");

    for (auto& item : items)
    {
        dropdown->addItem(item, item);
    }

    dropdown->setSelectedItem(selected);
    dropdown->onItemSelect(onSelect);

    return dropdown;
}

tgui::EditBox::Ptr WidgetBuilder::createTextInput(
    const std::string& initialValue,
    std::function<void(tgui::String)> onChange,
    const std::string& regexValidator)
{
    auto&& box = tgui::EditBox::create();
    box->setSize("100%", "80%");
    box->setPosition("0%", "10%");
    if (!regexValidator.empty()) box->setInputValidator(regexValidator);
    box->setText(initialValue);
    box->onTextChange(onChange);
    return box;
}