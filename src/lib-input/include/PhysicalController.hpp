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
        Printscreen,
        Confirm,
        Cancel,
        CursorLeft,
        CursorRight,
        CursorUp,
        CursorDown,
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

    [[nodiscard]] bool isEscapePressed() const
    {
        return testInputAndImmediatelyReleaseIt(InputCode::Escape);
    }

    [[nodiscard]] bool shouldTakeScreenshot() const
    {
        return testInputAndImmediatelyReleaseIt(InputCode::Printscreen);
    }

    [[nodiscard]] bool isConfirmPressed() const
    {
        return testInputAndImmediatelyReleaseIt(InputCode::Confirm);
    }

    [[nodiscard]] bool isCancelPressed() const
    {
        return testInputAndImmediatelyReleaseIt(InputCode::Cancel);
    }

    float getThrust() const override;

    float getSidewardThrust() const override;

    [[nodiscard]] sf::Vector2f getCursorDelta() const;

    float getSteer() const override;

    void updateSettings(const InputOptions& options);

private: // Setters
    void setMouseSensitivity(float value);

    [[nodiscard]] bool testInputAndImmediatelyReleaseIt(InputCode code) const;

private:
    const sf::Window& window;
    mutable dgm::Controller input;
    mutable sf::Vector2i lastMousePos;
    float mouseSensitivity = 25.f;
    float turnSpeed = 1.f;
    float cursorSensitivity_v2 = 5.f;
};
