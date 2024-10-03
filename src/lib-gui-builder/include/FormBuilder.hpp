#pragma once

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <string>
#include <tuple>
#include <vector>

class [[nodiscard]] FormBuilder final
{
public:
    FormBuilder() = default;
    FormBuilder(const FormBuilder&) = delete;
    FormBuilder(FormBuilder&&) = delete;
    ~FormBuilder() = default;

public:
    struct OptionConfig
    {
        bool disabled = false;
        std::optional<std::string> tooltipText = {};
    };

    [[nodiscard]] FormBuilder& addOption(
        const std::string& labelText,
        tgui::Widget::Ptr widget,
        OptionConfig config = {});

    [[nodiscard]] FormBuilder& addOptionWithWidgetId(
        const std::string& labelText,
        tgui::Widget::Ptr widget,
        const std::string widgetId);

    [[nodiscard]] FormBuilder& addOptionWithSubmit(
        const std::string& labelText,
        tgui::Widget::Ptr widget,
        tgui::Button::Ptr submitBtn);

    [[nodiscard]] FormBuilder& addSeparator();

    [[nodiscard]] tgui::Panel::Ptr
    build(tgui::Color backgroundColor = tgui::Color::Transparent);

private:
    struct RowProps
    {
        bool separator = false;
        std::string label;
        tgui::Widget::Ptr widget;
        std::optional<std::string> widgetId = {};
        std::optional<tgui::Button::Ptr> submitBtn = {};
        std::optional<std::string> tooltipText = {};
    };

    std::vector<RowProps> rowsToBuild;
};
