#include "include/Camera.hpp"

void Camera::applyTransforms()
{
	canvas->setView(currentView);
}

void Camera::move(const sf::Vector2f& direction)
{
	currentView.move(direction);
	applyTransforms();
}

void Camera::resetZoom()
{
	auto backupPosition = currentView.getCenter();
	currentView = canvas->getDefaultView();
	currentView.setCenter(backupPosition);
	applyTransforms();
}

void Camera::resetPosition()
{
	currentView.setCenter(canvas->getDefaultView().getCenter());
	applyTransforms();
}

sf::Vector2f Camera::getWorldCoordinates(const sf::Vector2f& viewCoordinates) const
{
	auto canvasPos = canvas->getPosition();
	auto difference = currentView.getCenter() - canvas->getDefaultView().getCenter() * currentZoom - canvas->getPosition() * currentZoom;
	auto result = difference + viewCoordinates * currentZoom;

	return result;
}

void Camera::zoom(float amount)
{
	resetZoom();

	currentZoom = std::clamp(currentZoom + amount, 0.1f, 2.f);

	currentView.zoom(currentZoom);
	applyTransforms();
}
