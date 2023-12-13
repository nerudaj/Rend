#pragma once

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

import Memory;

class WidgetCreator
{
public:
    static [[nodiscard]] tgui::Label::Ptr
    createLabel(const std::string& text, unsigned fontSize);

    static [[nodiscard]] tgui::CheckBox::Ptr
    createCheckbox(bool checked, std::function<void(bool)> onChange);

    static [[nodiscard]] tgui::Slider::Ptr createSlider(
        float value,
        std::function<void(void)> onChange,
        float lo = 0.f,
        float hi = 100.f,
        float step = 1.f);

    static [[nodiscard]] tgui::ComboBox::Ptr createDropdown(
        const std::vector<std::string>& items,
        const std::string& selected,
        std::function<void(void)> onSelect);

    static [[nodiscard]] tgui::EditBox::Ptr createTextInput(
        std::function<void(tgui::String)> onChange,
        const std::string& initialValue = "",
        const std::string& regexValidator = "");

    static [[nodiscard]] constexpr std::string getNumericValidator() noexcept
    {
        return "[1-9][0-9]*";
    }
};

class WidgetCreator2
{
public:
    static [[nodiscard]] tgui::CheckBox::Ptr
    createCheckbox(bool checked, std::function<void(bool)> onChange);

    static [[nodiscard]] tgui::Panel::Ptr createSlider(
        float value,
        std::function<void(float)> onChange,
        unsigned fontSize,
        const std::string& id,
        mem::Rc<tgui::Gui> gui,
        std::function<std::string(float)> valueFormatter,
        float lo = 0.f,
        float hi = 100.f,
        float step = 1.f);

    static [[nodiscard]] tgui::ComboBox::Ptr createDropdown(
        const std::vector<std::string>& items,
        const std::string& selected,
        std::function<void(std::size_t)> onSelect);
};

class GuiOptionsBuilder final
{
private:
    tgui::VerticalLayout::Ptr rowContainer;
    unsigned labelFontSize;
    std::vector<std::tuple<std::string, std::string, tgui::Widget::Ptr>>
        rowsToBuild;

public:
    [[nodiscard]] GuiOptionsBuilder(
        mem::Rc<tgui::Gui> gui,
        const tgui::Layout2d& pos,
        const tgui::Layout2d& size)
    {
        rowContainer = tgui::VerticalLayout::create(size);
        rowContainer->setPosition(pos);
        gui->add(rowContainer);
    }

    [[nodiscard]] GuiOptionsBuilder(
        tgui::Panel::Ptr panel,
        const tgui::Layout2d& pos,
        const tgui::Layout2d& size)
    {
        rowContainer = tgui::VerticalLayout::create(size);
        rowContainer->setPosition(pos);
        panel->add(rowContainer);
    }

public:
    [[nodiscard]] GuiOptionsBuilder& addOption(
        const std::string& name,
        const std::string& id,
        tgui::Widget::Ptr widget)
    {
        rowsToBuild.push_back({ name, id, widget });
        return *this;
    }

    void build();
};

class [[nodiscard]] GuiOptionsBuilder2 final
{
public:
    GuiOptionsBuilder2(tgui::Panel::Ptr panel) : panel(panel) {}

public:
    [[nodiscard]] GuiOptionsBuilder2&
    addOption(const std::string& labelText, tgui::Widget::Ptr widget)
    {
        rowsToBuild.push_back({ labelText, widget });
        return *this;
    }

    void build();

private:
    tgui::Panel::Ptr panel;
    unsigned labelFontSize;
    std::vector<std::tuple<std::string, tgui::Widget::Ptr>> rowsToBuild;
};
