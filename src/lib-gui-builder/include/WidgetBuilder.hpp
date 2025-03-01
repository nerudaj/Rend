#pragma once

#include <Memory.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <format>
#include <functional>
#include <string>

const tgui::Color PANEL_BACKGROUND_COLOR = tgui::Color(255, 255, 255, 64);

class WidgetBuilder final
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
    getStandardizedRow(tgui::Color bgcolor = tgui::Color::Transparent);

    static [[nodiscard]] tgui::Panel::Ptr createOptionRow(
        const std::string& labelText,
        tgui::Widget::Ptr widgetPtr,
        std::optional<std::string> widgetId);

    static [[nodiscard]] tgui::Panel::Ptr createOptionRowWithSubmitButton(
        const std::string& labelText,
        tgui::Widget::Ptr widgetPtr,
        tgui::Button::Ptr buttonPtr);

    static [[nodiscard]] tgui::Button::Ptr
    createButton(const std::string& label, std::function<void(void)> onClick);

    static [[nodiscard]] tgui::CheckBox::Ptr
    createCheckbox(bool checked, std::function<void(bool)> onChange);

    static [[nodiscard]] tgui::Panel::Ptr createSlider(
        float value,
        std::function<void(float)> onChange,
        tgui::Gui& gui,
        std::function<std::string(float)> valueFormatter,
        float lo = 0.f,
        float hi = 100.f,
        float step = 1.f);

    static [[nodiscard]] tgui::ComboBox::Ptr createDropdown(
        const std::vector<std::string>& items,
        const std::string& selected,
        std::function<void(std::size_t)> onSelect);

    static void updateDropdownItems(
        tgui::ComboBox::Ptr dropdown, const std::vector<std::string>& items);

    static [[nodiscard]] tgui::EditBox::Ptr createTextInput(
        const std::string& initialValue,
        std::function<void(tgui::String)> onChange,
        const std::string& regexValidator = "");

    template<std::integral Number>
    static [[nodiscard]] tgui::EditBox::Ptr createNumericInput(
        Number value,
        std::function<void(Number)> onChange,
        const std::string& validator = getUnsignedNumericValidator())
    {
        return createTextInput(
            std::to_string(value),
            [onChange](const tgui::String& newVal)
            {
                auto converted = std::stoul(newVal.toStdString());
                if (converted > std::numeric_limits<Number>::max())
                    throw std::runtime_error(std::format(
                        "Number is too big to fit {} bytes", sizeof(Number)));
                onChange(static_cast<Number>(converted));
            },
            validator);
    }

    static [[nodiscard]] tgui::Panel::Ptr createTabbedContent(
        const std::vector<std::string>& tabLabels,
        const std::string& contentPanelId,
        std::function<void(const tgui::String&)> onTabChange,
        std::optional<std::string> tabsId = std::nullopt);

    static [[nodiscard]] tgui::SeparatorLine::Ptr createSeparator();

    static [[nodiscard]] tgui::Label::Ptr
    createTooltip(const std::string& text);

    static [[nodiscard]] constexpr std::string
    getUnsignedNumericValidator() noexcept
    {
        return "(0|[1-9][0-9]*)";
    }

    static [[nodiscard]] constexpr std::string
    getPositiveNumericValidator() noexcept
    {
        return "[1-9][0-9]*";
    }
};
