#pragma once

#include <input/ControllerInterface.hpp>
#include <DGM/dgm.hpp>

class PhysicalController : public ControllerInterface {
public:
    enum InputCode
    {
        Forward,
        Backward,
        StrafeLeft,
        StrafeRight,
        TurnLeft,
        TurnRight
    };

public:
    PhysicalController();

public:
    float getThrust() const override;

    float getSidewardThrust() const override;

    float getSteer() const override;

private:
    dgm::Controller input;
};
