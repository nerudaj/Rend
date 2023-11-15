#pragma once

#include "include/Interfaces/ToolInterface.hpp"
#include "include/Interfaces/PenUserInterface.hpp"
#include "include/Utilities/DragContext.hpp"
#include "include/Utilities/GC.hpp"

class ToolWithDragAndSelect : public ToolInterface
{
public:
	ToolWithDragAndSelect(
		std::function<void(void)> onStateChanged,
		GC<ShortcutEngineInterface> shortcutEngine,
		GC<LayerObserverInterface> layerObserver)
		: ToolInterface(onStateChanged, shortcutEngine, layerObserver)
	{}

public:
	virtual void penDragStarted(const sf::Vector2i& start) final override;
	virtual void penDragUpdate(const sf::Vector2i& start, const sf::Vector2i& end) final override;
	virtual void penDragEnded(const sf::Vector2i& start, const sf::Vector2i& end) final override;
	virtual void penDragCancel(const sf::Vector2i& origin) override;
	virtual void penDelete() final override;

protected:
	[[nodiscard]]
	virtual std::optional<std::size_t> getObjectIndexFromMousePos(const sf::Vector2i& pos) const = 0;

	[[nodiscard]]
	virtual sf::Vector2i getPositionOfObjectWithIndex(std::size_t index) const = 0;
	virtual void selectObjectsInArea(const sf::IntRect& selectedArea) = 0;
	virtual void moveSelectedObjectsTo(const sf::Vector2i& point) = 0;
	virtual void createMoveCommand(
		const sf::Vector2i& src,
		const sf::Vector2i& dest) = 0;
	virtual void createDeleteCommand() = 0;

protected:
	std::set<std::size_t> selectedObjects;

	sf::RectangleShape selectMarker;
	DragContext dragContext;
	bool selecting = false;
	bool dragging = false;
};
