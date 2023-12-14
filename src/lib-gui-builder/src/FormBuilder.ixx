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
    FormBuilder(tgui::Panel::Ptr panel) : panel(panel) {}

public:
    [[nodiscard]] FormBuilder&
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
