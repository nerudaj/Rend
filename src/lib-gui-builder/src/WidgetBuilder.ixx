module;

#include <Configs/Sizers.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <functional>
#include <string>
#include <vector>

export module WidgetBuilder;

import Memory;

export const tgui::Color PANEL_BACKGROUND_COLOR =
    tgui::Color(255, 255, 255, 64);

export class WidgetBuilder final
{
private:
    static [[nodiscard]] tgui::Label::Ptr createLabelInternal(
        const std::string& text,
        const float sizeMultiplier,
        const bool justify = false);

public:
    static [[nodiscard]] tgui::Label::Ptr
    createTextLabel(const std::string& text)
    {
        return createLabelInternal(text, 1.f);
    }

    static [[nodiscard]] tgui::Label::Ptr createH2Label(const std::string& text)
    {
        return createLabelInternal(text, 2.f, true);
    }

    static [[nodiscard]] tgui::Label::Ptr createH1Label(const std::string& text)
    {
        return createLabelInternal(text, 4.f, true);
    }

    static [[nodiscard]] tgui::Panel::Ptr createPanel(
        const tgui::Layout2d& size = { "100%", "100%" },
        const tgui::Color color = tgui::Color::Transparent);

    static [[nodiscard]] tgui::Panel::Ptr
    getStandardizedRow(tgui::Color bgcolor = tgui::Color::Transparent)
    {
        auto&& row = tgui::Panel::create();
        row->setSize(
            "100%", std::to_string(Sizers::GetMenuBarHeight()).c_str());
        row->getRenderer()->setBackgroundColor(bgcolor);
        return row;
    }

    static [[nodiscard]] tgui::Panel::Ptr
    createOptionRow(const std::string& labelText, tgui::Widget::Ptr widgetPtr)
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
        widgetPanel->getRenderer()->setBackgroundColor(
            tgui::Color::Transparent);
        row->add(widgetPanel);
        widgetPanel->add(widgetPtr);

        return row;
    }

    static [[nodiscard]] tgui::Button::Ptr
    createButton(const std::string& label, std::function<void(void)> onClick);

    static [[nodiscard]] tgui::CheckBox::Ptr
    createCheckbox(bool checked, std::function<void(bool)> onChange)
    {
        auto&& checkbox = tgui::CheckBox::create();
        auto&& size = checkbox->getSizeLayout();
        checkbox->setPosition(("100%" - size.x) / 2, ("100%" - size.y) / 2);
        checkbox->setChecked(checked);
        checkbox->onChange(onChange);
        return checkbox;
    }

    static [[nodiscard]] tgui::Panel::Ptr createSlider(
        float value,
        std::function<void(float)> onChange,
        mem::Rc<tgui::Gui> gui,
        std::function<std::string(float)> valueFormatter,
        float lo = 0.f,
        float hi = 100.f,
        float step = 1.f);

    static [[nodiscard]] tgui::ComboBox::Ptr createDropdown(
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

    static [[nodiscard]] tgui::EditBox::Ptr createTextInput(
        const std::string& initialValue,
        std::function<void(tgui::String)> onChange,
        const std::string& regexValidator = "")
    {
        auto&& box = tgui::EditBox::create();
        box->setSize("100%", "80%");
        box->setPosition("0%", "10%");
        if (!regexValidator.empty()) box->setInputValidator(regexValidator);
        box->setText(initialValue);
        box->onTextChange(onChange);
        return box;
    }

    static [[nodiscard]] constexpr std::string getNumericValidator() noexcept
    {
        return "[1-9][0-9]*";
    }
};
