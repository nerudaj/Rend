#include "Tools/ToolWithDragAndSelect.hpp"

void ToolWithDragAndSelect::penDragStarted(const sf::Vector2i& start)
{
    auto itemId = getObjectIndexFromMousePos(start);
    if (itemId.has_value())
    {
        dragging = true;
        dragContext.leadingItemId = *itemId;

        if (not selectedObjects.contains(dragContext.leadingItemId))
            selectedObjects.clear();
        selectedObjects.insert(dragContext.leadingItemId);

        dragContext.initialDragOffset =
            getPositionOfObjectWithIndex(dragContext.leadingItemId) - start;
    }
    else
    {
        selecting = true;
        selectMarker.setPosition(sf::Vector2f(start));
    }
}

void ToolWithDragAndSelect::penDragUpdate(
    const sf::Vector2i& start, const sf::Vector2i& end)
{
    if (dragging)
    {
        moveSelectedObjectsTo(end);
    }

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

    if (dragging)
    {
        createMoveCommand(start, end);
    }

    dragging = false;
    selecting = false;
}

void ToolWithDragAndSelect::penDragCancel(const sf::Vector2i& origin)
{
    if (dragging)
    {
        moveSelectedObjectsTo(origin);
    }

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
