module;

#include <DGM/dgm.hpp>

export module PhysicalController;

import ControllerInterface;
import InputOptions;

export class [[nodiscard]] PhysicalController final : public ControllerInterface
{
public:
    enum InputCode
    {
        Forward,
        Backward,
        StrafeLeft,
        StrafeRight,
        TurnLeft,
        TurnRight,
        Shoot,
        PreviousWeapon,
        NextWeapon,
        LastWeapon,
        Escape
    };

public:
    PhysicalController(const sf::Window& window) : window(window);

public:
    void update() override;

    bool isGamepadConnected() const override;

    bool isShooting() const override;

    bool shouldSwapToPreviousWeapon() const override;

    bool shouldSwapToNextWeapon() const override;

    bool shouldSwapToLastWeapon() const override;

    bool isEscapePressed() const;

    float getThrust() const override;

    float getSidewardThrust() const override;

    float getSteer() const override;

    void updateSettings(const InputOptions& options);

private: // Setters
    void setMouseSensitivity(float value);

private:
    const sf::Window& window;
    mutable dgm::Controller input;
    mutable sf::Vector2i lastMousePos;
    float mouseSensitivity = 25.f;
    float turnSpeed = 1.f;
};

module : private;

PhysicalController::PhysicalController(const sf::Window& window) : window(window)
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

void PhysicalController::update()
{
    input.update();
}

bool PhysicalController::isGamepadConnected() const 
{
    return input.isControllerConnected();
}

bool PhysicalController::isShooting() const override
{
    return input.isInputToggled(InputCode::Shoot);
}

bool PhysicalController::shouldSwapToPreviousWeapon() const override
{
    const bool result = input.isInputToggled(InputCode::PreviousWeapon);
    if (result) input.releaseInput(InputCode::PreviousWeapon);
    return result;
}

bool PhysicalController::shouldSwapToNextWeapon() const override
{
    const bool result = input.isInputToggled(InputCode::NextWeapon);
    if (result) input.releaseInput(InputCode::NextWeapon);
    return result;
}

bool PhysicalController::shouldSwapToLastWeapon() const override
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

float PhysicalController::getThrust() const override
{
    return input.getInputValue(InputCode::Forward)
           + input.getInputValue(InputCode::Backward);
}

float PhysicalController::getSidewardThrust() const override
{
    return input.getInputValue(InputCode::StrafeLeft)
           + input.getInputValue(InputCode::StrafeRight);
}

float PhysicalController::getSteer() const override
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
}

void PhysicalController::setMouseSensitivity(float value)
{
    // Value needs to be inverted
    mouseSensitivity = 50.f - value;
}
