#pragma once

#include "include/Interfaces/PenUserInterface.hpp"
#include "include/Interfaces/PhysicalPenInterface.hpp"

#include <functional>

class PhysicalPen final : public PhysicalPenInterface
{
public:
	PhysicalPen(std::function<PenUserInterface& ()> getPenUser)
		: getPenUser(getPenUser)
	{}

public:
	[[nodiscard]]
	constexpr const sf::Vector2i& getCurrentPenPos() const noexcept
	{
		return currentPenPosition;
	}

public: // PhysicalPenInterface
	void updatePenPosition(const sf::Vector2i& position) override;

	void penUp() override;

	void penDown() override;

	void penCancel() override;

	void penDelete() override;

private:
	std::function<PenUserInterface& ()> getPenUser;
	sf::Vector2i currentPenPosition = { 0, 0 };
	sf::Vector2i penPositionBeforeDrag = { 0, 0 };
	bool penIsDown = false;
	bool dragging = false;
};
