#include "Tools/ToolWithDragAndSelect.hpp"

void ToolWithDragAndSelect::penDragStarted(const sf::Vector2i& start)
{
    selecting = true;
    selectMarker.setPosition(sf::Vector2f(start));
}

void ToolWithDragAndSelect::penDragUpdate(
    const sf::Vector2i& start, const sf::Vector2i& end)
{
    if (selecting)
    {
        selectMarker.setSize(sf::Vector2f(end - start));
    }
}

void ToolWithDragAndSelect::penDragEnded(
    const sf::Vector2i& start, const sf::Vector2i& end)
{
    if (selecting)
    {
        selectedObjects.clear();
        sf::IntRect selectedArea(
            Helper::minVector(start, end),
            { std::abs(start.x - end.x), std::abs(start.y - end.y) });
        selectObjectsInArea(selectedArea);
    }

    selecting = false;
}

void ToolWithDragAndSelect::penDragCancel(const sf::Vector2i& origin)
{
    selectedObjects.clear();
    dragging = false;
    selecting = false;
}

void ToolWithDragAndSelect::penDelete()
{
    [[unlikely]] if (selectedObjects.empty())
        return;

    createDeleteCommand();

    selectedObjects.clear();
}
