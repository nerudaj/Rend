#pragma once

#include <SFML/System/Vector2.hpp>

class PenUserInterface
{
public:
	virtual ~PenUserInterface() = default;

public:
	virtual void penClicked(const sf::Vector2i& position) = 0;
	virtual void penDragStarted(const sf::Vector2i& start) = 0;
	virtual void penDragUpdate(
		const sf::Vector2i& start,
		const sf::Vector2i& end) = 0;
	virtual void penDragEnded(
		const sf::Vector2i& start,
		const sf::Vector2i& end) = 0;
	virtual void penDragCancel(const sf::Vector2i& origin) = 0;
	virtual void penDelete() = 0;
};
