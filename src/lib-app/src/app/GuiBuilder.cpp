#include "app/GuiBuilder.hpp"
#include "builder/WidgetBuilder.hpp"

void GuiOptionsBuilder2::build()
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
