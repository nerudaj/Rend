#include "TableBuilder.hpp"
#include "WidgetBuilder.hpp"
#include <Configs/Sizers.hpp>
#include <cassert>
#include <ranges>

void priv::TableBuilder::addRow(const std::vector<std::string>& cells)
{
    assert(!heading || heading->size() == cells.size());
    assert(rowsOfCells.empty() || rowsOfCells.front().size() == cells.size());
    rowsOfCells.push_back(cells);
}

template<class Range>
[[nodiscard]] static auto enumerate(Range&& range)
{
    return std::views::zip(
        std::views::iota(size_t { 0 }, std::ranges::size(range)),
        std::forward<Range>(range));
}

[[nodiscard]] static tgui::Label::Ptr
createCell(const std::string& str, size_t column, size_t totalColumns)
{
    size_t columnWidth = 100 / totalColumns;

    auto label = tgui::Label::create(str);
    label->getRenderer()->setTextColor(tgui::Color::Black);
    label->setSize({ tgui::Layout(std::to_string(columnWidth) + "%"), "100%" });
    label->setPosition(
        { tgui::Layout(std::to_string(column * columnWidth) + "%"), "0%" });
    label->setTextSize(Sizers::GetMenuBarTextHeight());
    label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    return label;
}

tgui::Panel::Ptr priv::TableBuilder::build()
{
    auto&& panel = WidgetBuilder::createPanel();

    assert(heading || !rowsOfCells.empty());

    const auto&& columnCount =
        heading ? heading->size() : rowsOfCells.front().size();

    if (heading)
    {
        auto&& row = WidgetBuilder::getStandardizedRow(tgui::Color::White);

        for (auto&& [columnIdx, cellText] : enumerate(heading.value()))
        {
            row->add(createCell(cellText, columnIdx, columnCount));
        }

        panel->add(row);
    }

    auto&& rowIdx = heading ? 1u : 0u;

    for (auto&& row : rowsOfCells)
    {
        auto&& color = rowIdx % 2 == 0 ? tgui::Color(255, 255, 255, 128)
                                       : tgui::Color(255, 255, 255, 192);
        auto&& rowWidget = WidgetBuilder::getStandardizedRow(color);
        rowWidget->setPosition({ "0%", rowWidget->getSize().y * rowIdx++ });

        for (auto&& [columnIdx, cellText] : enumerate(row))
        {
            rowWidget->add(createCell(cellText, columnIdx, columnCount));
        }

        panel->add(rowWidget);
    }

    return panel;
}
