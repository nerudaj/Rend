import FormBuilder;
import WidgetBuilder;

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <string>
#include <tuple>
#include <vector>

void FormBuilder::build()
{
    unsigned rowIdx = 0;

    for (auto&& [labelText, widgetPtr] : rowsToBuild)
    {
        auto row = WidgetBuilder::createOptionRow(labelText, widgetPtr);
        row->setPosition("0%", row->getSize().y * rowIdx);
        panel->add(row);
        ++rowIdx;
    }
}
