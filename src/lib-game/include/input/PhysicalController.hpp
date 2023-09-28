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
        Shoot
    };

public:
    PhysicalController();

public:
    bool isShooting() const override;

    float getThrust() const override;

    float getSidewardThrust() const override;

    float getSteer() const override;

private:
    dgm::Controller input;
};
