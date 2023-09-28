#include <input/PhysicalController.hpp>

PhysicalController::PhysicalController()
{
    input.bindInput(InputCode::Forward, sf::Keyboard::W);
    input.bindInput(InputCode::Backward, sf::Keyboard::S);
    input.bindInput(InputCode::StrafeLeft, sf::Keyboard::A);
    input.bindInput(InputCode::StrafeRight, sf::Keyboard::D);
    input.bindInput(InputCode::TurnLeft, sf::Keyboard::Left);
    input.bindInput(InputCode::TurnRight, sf::Keyboard::Right);
    input.bindInput(InputCode::Shoot, sf::Keyboard::Space);
}

bool PhysicalController::isShooting() const
{
    return input.isToggled(InputCode::Shoot);
}

float PhysicalController::getThrust() const
{
    return input.getValue(InputCode::Forward)
           - input.getValue(InputCode::Backward);
}

float PhysicalController::getSidewardThrust() const
{
    return -input.getValue(InputCode::StrafeLeft)
           + input.getValue(InputCode::StrafeRight);
}

float PhysicalController::getSteer() const
{
    return -input.getValue(InputCode::TurnLeft)
           + input.getValue(InputCode::TurnRight);
}
