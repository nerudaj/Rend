#pragma once

#include <DGM/dgm.hpp>
#include <input/ControllerInterface.hpp>

class PhysicalController : public ControllerInterface
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
    PhysicalController();

public:
    bool isShooting() const override;

    bool shouldSwapToPreviousWeapon() const override;

    bool shouldSwapToNextWeapon() const override;

    bool shouldSwapToLastWeapon() const override;

    float getThrust() const override;

    float getSidewardThrust() const override;

    float getSteer() const override;

private:
    mutable dgm::Controller input;
};
