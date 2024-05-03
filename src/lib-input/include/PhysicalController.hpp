#pragma once

#include "ControllerInterface.hpp"
#include "InputOptions.hpp"
#include <DGM/dgm.hpp>

class [[nodiscard]] PhysicalController final : public ControllerInterface
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
        Escape,
        Printscreen
    };

public:
    PhysicalController(const sf::Window& window);

public:
    void update() override;

    bool isGamepadConnected() const override;

    bool isShooting() const override;

    bool shouldSwapToPreviousWeapon() const override;

    bool shouldSwapToNextWeapon() const override;

    bool shouldSwapToLastWeapon() const override;

    bool isEscapePressed() const;

    bool shouldTakeScreenshot() const;

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
