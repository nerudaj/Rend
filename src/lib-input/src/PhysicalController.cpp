import PhysicalController;

#include <DGM/dgm.hpp>

PhysicalController::PhysicalController(const sf::Window& window)
    : window(window)
{
    input.bindInput(InputCode::Forward, sf::Keyboard::W);
    input.bindInput(InputCode::Backward, sf::Keyboard::S);
    input.bindInput(InputCode::StrafeLeft, sf::Keyboard::A);
    input.bindInput(InputCode::StrafeRight, sf::Keyboard::D);
    input.bindInput(InputCode::TurnLeft, sf::Keyboard::Left);
    input.bindInput(InputCode::TurnRight, sf::Keyboard::Right);
    input.bindInput(InputCode::Shoot, sf::Keyboard::Space);
    input.bindInput(InputCode::Shoot, sf::Mouse::Left);
    input.bindInput(InputCode::PreviousWeapon, sf::Keyboard::Q);
    input.bindInput(InputCode::NextWeapon, sf::Keyboard::E);
    input.bindInput(InputCode::LastWeapon, sf::Keyboard::R);
    input.bindInput(InputCode::LastWeapon, sf::Mouse::Right);
    input.bindInput(InputCode::Escape, sf::Keyboard::Escape);

    input.setGamepadIndex(0);
    input.bindInput(InputCode::Forward, dgm::Xbox::Axis::LStickYpos);
    input.bindInput(InputCode::Backward, dgm::Xbox::Axis::LStickYneg);
    input.bindInput(InputCode::StrafeLeft, dgm::Xbox::Axis::LStickXneg);
    input.bindInput(InputCode::StrafeRight, dgm::Xbox::Axis::LStickXpos);
    input.bindInput(InputCode::TurnLeft, dgm::Xbox::Axis::RStickXneg);
    input.bindInput(InputCode::TurnRight, dgm::Xbox::Axis::RStickXpos);
    input.bindInput(InputCode::PreviousWeapon, dgm::Xbox::Button::LBumper);
    input.bindInput(InputCode::NextWeapon, dgm::Xbox::Button::RBumper);
    input.bindInput(InputCode::LastWeapon, dgm::Xbox::Axis::LTrigger);
    input.bindInput(InputCode::LastWeapon, dgm::Xbox::Button::Y);
    input.bindInput(InputCode::Shoot, dgm::Xbox::Axis::RTrigger);
    input.bindInput(InputCode::Shoot, dgm::Xbox::Button::A);
    input.bindInput(InputCode::Escape, dgm::Xbox::Button::Back);
}

bool PhysicalController::isShooting() const
{
    return input.isInputToggled(InputCode::Shoot);
}

bool PhysicalController::shouldSwapToPreviousWeapon() const
{
    const bool result = input.isInputToggled(InputCode::PreviousWeapon);
    if (result) input.releaseInput(InputCode::PreviousWeapon);
    return result;
}

bool PhysicalController::shouldSwapToNextWeapon() const
{
    const bool result = input.isInputToggled(InputCode::NextWeapon);
    if (result) input.releaseInput(InputCode::NextWeapon);
    return result;
}

bool PhysicalController::shouldSwapToLastWeapon() const
{
    const bool result = input.isInputToggled(InputCode::LastWeapon);
    if (result) input.releaseInput(InputCode::LastWeapon);
    return result;
}

bool PhysicalController::isEscapePressed() const
{
    const bool result = input.isInputToggled(InputCode::Escape);
    if (result) input.releaseInput(InputCode::Escape);
    return result;
}

float PhysicalController::getThrust() const
{
    return input.getInputValue(InputCode::Forward)
           + input.getInputValue(InputCode::Backward);
}

float PhysicalController::getSidewardThrust() const
{
    return input.getInputValue(InputCode::StrafeLeft)
           + input.getInputValue(InputCode::StrafeRight);
}

float PhysicalController::getSteer() const
{
    const auto windowWidthHalf = window.getSize().x / 2.f;
    const auto position = sf::Mouse::getPosition(window);
    const auto xDiff = (position.x - windowWidthHalf);

    return xDiff / mouseSensitivity
           + (input.getInputValue(InputCode::TurnLeft)
              + input.getInputValue(InputCode::TurnRight))
                 * turnSpeed;
}

void PhysicalController::updateSettings(const InputOptions& options)
{
    setMouseSensitivity(options.mouseSensitivity);
    turnSpeed = options.turnSpeed;
    input.setControllerDeadzone(options.gamepadDeadzone);
}