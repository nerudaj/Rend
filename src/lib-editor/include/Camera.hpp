#pragma once

#include <DGM/dgm.hpp>
#include <TGUI/TGUI.hpp>

class Camera {
private:
	tgui::Canvas::Ptr &canvas;
	sf::View currentView;
	float currentZoom = 1.f;

	void applyTransforms();

public:
	void move(const sf::Vector2f& direction);

	void zoom(float amount);

	void resetZoom();

	void resetPosition();

	sf::Vector2f getWorldCoordinates(const sf::Vector2f& viewCoordinates) const;

	void init() {
		currentView = canvas->getView();
	}

	float getCurrentZoomLevel() const {
		return currentZoom;
	}

	Camera(tgui::Canvas::Ptr &canvas) : canvas(canvas) {}
};

