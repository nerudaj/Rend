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
    };

public:
    PhysicalController(const sf::Window& window);

public:
    void update() override
    {
        input.update();
    }

    bool isGamepadConnected() const override
    {
        return input.isControllerConnected();
    }

    bool isShooting() const override;

    bool shouldSwapToPreviousWeapon() const override;

    bool shouldSwapToNextWeapon() const override;

    bool shouldSwapToLastWeapon() const override;

    float getThrust() const override;

    float getSidewardThrust() const override;

    float getSteer() const override;

    void updateSettings(const InputOptions& options);

private: // Setters
    void setMouseSensitivity(float value)
    {
        // Value needs to be inverted
        mouseSensitivity = 50.f - value;
    }

private:
    const sf::Window& window;
    mutable dgm::Controller input;
    mutable sf::Vector2i lastMousePos;
    float mouseSensitivity = 25.f;
    float turnSpeed = 1.f;
};
