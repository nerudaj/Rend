#include "PhysicalController.hpp"

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
    input.bindInput(InputCode::Printscreen, sf::Keyboard::P);

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
    input.bindInput(InputCode::Confirm, dgm::Xbox::Button::A);
    input.bindInput(InputCode::Cancel, dgm::Xbox::Button::B);
    input.bindInput(InputCode::CursorLeft, dgm::Xbox::Axis::LStickXneg);
    input.bindInput(InputCode::CursorRight, dgm::Xbox::Axis::LStickXpos);
    input.bindInput(InputCode::CursorUp, dgm::Xbox::Axis::LStickYneg);
    input.bindInput(InputCode::CursorDown, dgm::Xbox::Axis::LStickYpos);
}

void PhysicalController::update()
{
    input.update();
}

bool PhysicalController::isGamepadConnected() const
{
    return input.isControllerConnected();
}

bool PhysicalController::isShooting() const
{
    return input.isInputToggled(InputCode::Shoot);
}

bool PhysicalController::shouldSwapToPreviousWeapon() const
{
    return testInputAndImmediatelyReleaseIt(InputCode::PreviousWeapon);
}

bool PhysicalController::shouldSwapToNextWeapon() const
{
    return testInputAndImmediatelyReleaseIt(InputCode::NextWeapon);
}

bool PhysicalController::shouldSwapToLastWeapon() const
{
    return testInputAndImmediatelyReleaseIt(InputCode::LastWeapon);
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

sf::Vector2f PhysicalController::getCursorDelta() const
{
    return sf::Vector2f(
               input.getInputValue(InputCode::CursorLeft)
                   + input.getInputValue(InputCode::CursorRight),
               -input.getInputValue(InputCode::CursorUp)
                   - input.getInputValue(InputCode::CursorDown))
           * cursorSensitivity_v2;
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
    input.setGamepadDeadzone(options.gamepadDeadzone);
    cursorSensitivity_v2 = options.cursorSensitivity_v2;
}

void PhysicalController::setMouseSensitivity(float value)
{
    // Value needs to be inverted
    mouseSensitivity = 50.f - value;
}

bool PhysicalController::testInputAndImmediatelyReleaseIt(InputCode code) const
{
    const bool result = input.isInputToggled(code);
    if (result) input.releaseInput(code);
    return result;
}
