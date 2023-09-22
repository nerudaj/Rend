#include "app/GuiBuilder.hpp"

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

void GuiOptionsBuilder::build()
{
    auto size = rowContainer->getSize();
    labelFontSize = size.y / rowsToBuild.size() * 0.8f;

    for (auto&& [name, id, widget] : rowsToBuild)
    {
        auto row = tgui::HorizontalLayout::create();
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
