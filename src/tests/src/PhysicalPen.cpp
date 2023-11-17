#include <catch.hpp>
#include <fakeit.hpp>
#include "include/Tools/PhysicalPen.hpp"

using namespace fakeit;

TEST_CASE("[PhysicalPen]")
{
	Mock<PenUserInterface> penUserMock;
	PhysicalPen physicalPen([&] () -> PenUserInterface&
		{
			return penUserMock.get();
		});

	SECTION("Invokes delete on penDelete")
	{
		When(Method(penUserMock, penDelete)).Return();
		physicalPen.penDelete();
		Verify(Method(penUserMock, penDelete));
	}

	SECTION("Invokes cancel on penCancel")
	{
		When(Method(penUserMock, penDragCancel)).Return();
		physicalPen.penCancel();
		Verify(Method(penUserMock, penDragCancel));
	}

	SECTION("Invokes click on down/up")
	{
		When(Method(penUserMock, penClicked)).Return();
		physicalPen.penDown();
		physicalPen.penUp();
		Verify(Method(penUserMock, penClicked));
	}

	SECTION("Pen drag works (start, update, end)")
	{
		physicalPen.updatePenPosition(sf::Vector2i(10, 10));

		When(Method(penUserMock, penDragStarted)).Return();
		physicalPen.penDown();
		physicalPen.updatePenPosition(sf::Vector2i(20, 20));
		Verify(Method(penUserMock, penDragStarted));

		When(Method(penUserMock, penDragUpdate)).Return();
		physicalPen.updatePenPosition(sf::Vector2i(30, 30));
		Verify(Method(penUserMock, penDragUpdate));

		When(Method(penUserMock, penDragEnded)).Return();
		physicalPen.penUp();
		Verify(Method(penUserMock, penDragEnded));
	}
}
