#pragma once

#include "ControllerInterface.hpp"

class NullController : public ControllerInterface
{
public:
    bool isShooting() const override
    {
        return true;
    }

    bool shouldSwapToPreviousWeapon() const override
    {
        return false;
    }

    bool shouldSwapToNextWeapon() const override
    {
        return false;
    }

    bool shouldSwapToLastWeapon() const override
    {
        return false;
    }

    float getThrust() const override
    {
        return 0.f;
    }

    float getSidewardThrust() const override
    {
        return 0.f;
    }

    float getSteer() const override
    {
        return 0.f;
    }
};
