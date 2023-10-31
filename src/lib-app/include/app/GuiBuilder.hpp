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

class GuiOptionsBuilder final
{
private:
    mem::Rc<tgui::Gui> gui;
    tgui::VerticalLayout::Ptr rowContainer;
    unsigned labelFontSize;
    std::vector<std::tuple<std::string, std::string, tgui::Widget::Ptr>>
        rowsToBuild;

public:
    [[nodiscard]] GuiOptionsBuilder(
        mem::Rc<tgui::Gui> gui,
        const tgui::Layout2d& pos,
        const tgui::Layout2d& size)
        : gui(gui)
    {
        rowContainer = tgui::VerticalLayout::create(size);
        rowContainer->setPosition(pos);
        gui->add(rowContainer);
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