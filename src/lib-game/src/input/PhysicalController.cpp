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
    input.bindInput(InputCode::PreviousWeapon, sf::Keyboard::Q);
    input.bindInput(InputCode::NextWeapon, sf::Keyboard::E);
    input.bindInput(InputCode::LastWeapon, sf::Keyboard::R);
}

bool PhysicalController::isShooting() const
{
    return input.isToggled(InputCode::Shoot);
}

bool PhysicalController::shouldSwapToPreviousWeapon() const
{
    const bool result = input.isToggled(InputCode::PreviousWeapon);
    if (result) input.releaseKey(InputCode::PreviousWeapon);
    return result;
}

bool PhysicalController::shouldSwapToNextWeapon() const
{
    const bool result = input.isToggled(InputCode::NextWeapon);
    if (result) input.releaseKey(InputCode::NextWeapon);
    return result;
}

bool PhysicalController::shouldSwapToLastWeapon() const
{
    const bool result = input.isToggled(InputCode::LastWeapon);
    if (result) input.releaseKey(InputCode::LastWeapon);
    return result;
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
