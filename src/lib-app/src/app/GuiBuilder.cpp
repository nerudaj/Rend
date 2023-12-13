#include "app/GuiBuilder.hpp"
#include <Configs/Sizers.hpp>

tgui::Label::Ptr
WidgetCreator::createLabel(const std::string& text, unsigned fontSize)
{
    auto label = tgui::Label::create(text);
    label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
    label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    label->setAutoSize(false);
    label->setTextSize(fontSize);
    return label;
}

tgui::CheckBox::Ptr
WidgetCreator::createCheckbox(bool checked, std::function<void(bool)> onChange)
{
    auto checkbox = tgui::CheckBox::create();
    checkbox->setChecked(checked);
    checkbox->onChange(onChange);
    return checkbox;
}

tgui::Slider::Ptr WidgetCreator::createSlider(
    float value,
    std::function<void(void)> onChange,
    float lo,
    float hi,
    float step)
{
    auto slider = tgui::Slider::create(lo, hi);

    slider->setStep(step);
    slider->setValue(value);
    slider->onValueChange(onChange);

    return slider;
}

tgui::ComboBox::Ptr WidgetCreator::createDropdown(
    const std::vector<std::string>& items,
    const std::string& selected,
    std::function<void(void)> onSelect)
{
    auto dropdown = tgui::ComboBox::create();
    for (auto& item : items)
    {
        dropdown->addItem(item, item);
    }
    dropdown->setSelectedItem(selected);
    dropdown->onItemSelect(onSelect);

    return dropdown;
}

tgui::EditBox::Ptr WidgetCreator::createTextInput(
    std::function<void(tgui::String)> onChange,
    const std::string& initialValue,
    const std::string& regexValidator)
{
    auto box = tgui::EditBox::create();
    if (!regexValidator.empty()) box->setInputValidator(regexValidator);
    box->setText(initialValue);
    box->onTextChange(onChange);
    return box;
}

void GuiOptionsBuilder::build()
{
    auto size = rowContainer->getSize();
    labelFontSize = static_cast<unsigned>(size.y / rowsToBuild.size() * 0.8f);

    for (auto&& [name, id, widget] : rowsToBuild)
    {
        auto row = tgui::HorizontalLayout::create();
        row->setSize("100%", "5%");
        rowContainer->add(row);

        auto label = WidgetCreator::createLabel(name, labelFontSize);
        row->add(label);
        row->setRatio(0, 0.5f);

        row->add(widget, id);

        if (id.starts_with("Checkbox"))
        {
            row->setRatio(1, 0.1f);
            row->insertSpace(2, 0.4f);
        }
        else
        {
            row->setRatio(1, 0.5f);
        }
    }
}

void GuiOptionsBuilder2::build()
{
    unsigned rowIdx = 0;
    auto rowHeight = Sizers::GetMenuBarHeight();

    for (auto&& [labelText, widgetPtr] : rowsToBuild)
    {
        auto row = tgui::Panel::create();
        row->setSize("100%", std::to_string(rowHeight).c_str());
        row->setPosition("0%", std::to_string(rowHeight * rowIdx++).c_str());
        panel->add(row);

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
    }
}

tgui::CheckBox::Ptr
WidgetCreator2::createCheckbox(bool checked, std::function<void(bool)> onChange)
{
    auto checkbox = tgui::CheckBox::create();
    auto size = checkbox->getSizeLayout();
    checkbox->setPosition(("100%" - size.x) / 2, ("100%" - size.y) / 2);
    checkbox->setChecked(checked);
    checkbox->onChange(onChange);
    return checkbox;
}

tgui::Panel::Ptr WidgetCreator2::createSlider(
    float value,
    std::function<void(float)> onChange,
    unsigned fontSize,
    const std::string& id,
    mem::Rc<tgui::Gui> gui,
    std::function<std::string(float)> valueFormatter,
    float lo,
    float hi,
    float step)
{
    auto result = tgui::Panel::create();

    auto valueLabel = tgui::Label::create(valueFormatter(value));
    valueLabel->setTextSize(fontSize);
    valueLabel->setPosition("85%", "0%");
    valueLabel->setSize("15%", "100%");
    valueLabel->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    valueLabel->setHorizontalAlignment(
        tgui::Label::HorizontalAlignment::Center);
    result->add(valueLabel, id + "Label");

    auto slider = tgui::Slider::create(lo, hi);

    slider->setPosition("0%", "25%");
    slider->setSize("85%", "50%");
    slider->setStep(step);
    slider->setValue(value);
    slider->onValueChange(
        [gui, id, onChange, valueFormatter](float value)
        {
            gui->get<tgui::Label>(id + "Label")->setText(valueFormatter(value));
            onChange(value);
        });
    result->add(slider);

    return result;
}

tgui::ComboBox::Ptr WidgetCreator2::createDropdown(
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
