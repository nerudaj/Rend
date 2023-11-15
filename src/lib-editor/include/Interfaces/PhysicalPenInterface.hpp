#pragma once

#include <SFML/System/Vector2.hpp>

class PhysicalPenInterface
{
public:
	virtual ~PhysicalPenInterface() = default;

public:
	virtual void updatePenPosition(const sf::Vector2i& position) = 0;
	virtual void penUp() = 0;
	virtual void penDown() = 0;
	virtual void penCancel() = 0;
	virtual void penDelete() = 0;
};
