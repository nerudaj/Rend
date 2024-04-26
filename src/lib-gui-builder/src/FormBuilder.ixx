module;

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <string>
#include <tuple>
#include <vector>

export module FormBuilder;

export class [[nodiscard]] FormBuilder final
{
public:
    FormBuilder() = default;
    FormBuilder(const FormBuilder&) = delete;
    FormBuilder(FormBuilder&&) = delete;
    ~FormBuilder() = default;

public:
    [[nodiscard]] FormBuilder& addOption(
        const std::string& labelText,
        tgui::Widget::Ptr widget,
        bool disabled = false);

    [[nodiscard]] FormBuilder& addOptionWithWidgetId(
        const std::string& labelText,
        tgui::Widget::Ptr widget,
        const std::string widgetId);

    [[nodiscard]] FormBuilder& addOptionWithSubmit(
        const std::string& labelText,
        tgui::Widget::Ptr widget,
        tgui::Button::Ptr submitBtn);

    [[nodiscard]] tgui::Panel::Ptr
    build(tgui::Color backgroundColor = tgui::Color::Transparent);

private:
    struct RowProps
    {
        std::string label;
        tgui::Widget::Ptr widget;
        std::optional<std::string> widgetId = {};
        std::optional<tgui::Button::Ptr> submitBtn = {};
    };

    std::vector<RowProps> rowsToBuild;
};
