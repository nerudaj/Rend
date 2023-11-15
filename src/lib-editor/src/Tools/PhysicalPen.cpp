#include "include/Tools/PhysicalPen.hpp"
#include <DGM/dgm.hpp>

void PhysicalPen::updatePenPosition(const sf::Vector2i& position)
{
	// move by at least 3 pixels to trigger dragging
	constexpr float DRAG_THRESHOLD = 3.f;
	currentPenPosition = position;

	if (penIsDown && !dragging)
	{
		dragging = dgm::Math::vectorSize(sf::Vector2f(
			position - penPositionBeforeDrag)) > DRAG_THRESHOLD;

		if (dragging)
		{
			getPenUser().penDragStarted(penPositionBeforeDrag);
		}
	}
	else if (penIsDown && dragging)
	{
		getPenUser().penDragUpdate(penPositionBeforeDrag, position);
	}
}

void PhysicalPen::penUp()
{
	if (dragging)
	{
		getPenUser().penDragEnded(
			penPositionBeforeDrag,
			currentPenPosition);
	}
	else
	{
		getPenUser().penClicked(currentPenPosition);
	}

	dragging = false;
	penIsDown = false;
}

void PhysicalPen::penDown()
{
	penPositionBeforeDrag = currentPenPosition;
	penIsDown = true;
}

void PhysicalPen::penCancel()
{
	getPenUser().penDragCancel(penPositionBeforeDrag);
	penIsDown = false;
}

void PhysicalPen::penDelete()
{
	getPenUser().penDelete();
}
