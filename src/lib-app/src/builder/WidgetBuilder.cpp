#include "builder/WidgetBuilder.hpp"
#include <Configs/Sizers.hpp>
#include <ranges>

[[nodiscard]] std::string randomString(std::size_t len)
{
    constexpr const char* CHARS =
        "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return std::views::iota(0u, len)
           | std::views::transform([CHARS](std::size_t i)
                                   { return CHARS[rand() % std::size(CHARS)]; })
           | std::ranges::to<std::string>();
}

tgui::Panel::Ptr WidgetBuilder::createOptionRow(
    const std::string& labelText, tgui::Widget::Ptr widgetPtr)
{
    auto row = tgui::Panel::create();
    row->setSize("100%", std::to_string(Sizers::GetMenuBarHeight()).c_str());

    auto label = tgui::Label::create(labelText);
    label->setSize("60%", "100%");
    label->setPosition("0%", "0%");
    label->setTextSize(Sizers::GetMenuBarTextHeight());
    label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    row->add(label);

    auto widgetPanel = tgui::Panel::create({ "40%", "100%" });
    widgetPanel->setPosition("60%", "0%");
    row->add(widgetPanel);
    widgetPanel->add(widgetPtr);

    return row;
}

tgui::CheckBox::Ptr
WidgetBuilder::createCheckbox(bool checked, std::function<void(bool)> onChange)
{
    auto checkbox = tgui::CheckBox::create();
    auto size = checkbox->getSizeLayout();
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
    auto result = tgui::Panel::create();
    const auto&& ID = randomString(16);

    auto valueLabel = tgui::Label::create(valueFormatter(value));
    valueLabel->setTextSize(Sizers::GetMenuBarTextHeight());
    valueLabel->setPosition("85%", "0%");
    valueLabel->setSize("15%", "100%");
    valueLabel->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    valueLabel->setHorizontalAlignment(
        tgui::Label::HorizontalAlignment::Center);
    result->add(valueLabel, ID);

    auto slider = tgui::Slider::create(lo, hi);

    slider->setPosition("0%", "25%");
    slider->setSize("85%", "50%");
    slider->setStep(step);
    slider->setValue(value);
    slider->onValueChange(
        [gui, ID, onChange, valueFormatter](float value)
        {
            gui->get<tgui::Label>(ID)->setText(valueFormatter(value));
            onChange(value);
        });
    result->add(slider);

    return result;
}

tgui::ComboBox::Ptr WidgetBuilder::createDropdown(
    const std::vector<std::string>& items,
    const std::string& selected,
    std::function<void(std::size_t)> onSelect)
{
    auto dropdown = tgui::ComboBox::create();
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
    auto box = tgui::EditBox::create();
    box->setSize("100%", "80%");
    box->setPosition("0%", "10%");
    if (!regexValidator.empty()) box->setInputValidator(regexValidator);
    box->setText(initialValue);
    box->onTextChange(onChange);
    return box;
}