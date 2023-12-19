module;

#include <DGM/dgm.hpp>

import ControllerInterface;
export module PhysicalController;

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
    PhysicalController(const sf::Window& window, float mouseSensitivity);

public:
    void update() override
    {
        input.update(dgm::Time {});
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

    void setMouseSensitivity(float value);

private:
    const sf::Window& window;
    mutable dgm::Controller input;
    mutable sf::Vector2i lastMousePos;
    float mouseSensitivity = 25.f;
};
