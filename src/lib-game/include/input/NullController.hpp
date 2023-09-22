#pragma once

#include <input/ControllerInterface.hpp>

class NullController : public ControllerInterface
{
public:
    float getThrust() const override
    {
        return 0.f;
    }

    float getSidewardThrust() const override {
        return 0.f;
    }

    float getSteer() const override
    {
        return 0.f;
    }
};
