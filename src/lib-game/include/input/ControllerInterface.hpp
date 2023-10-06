#pragma once

#include <input/InputSchema.hpp>

class ControllerInterface
{
public:
    virtual ~ControllerInterface() = default;

public:
    [[nodiscard]] constexpr InputSchema getSnapshot() const
    {
        return InputSchema {
            .shooting = isShooting(),
            .prevWeapon = shouldSwapToPreviousWeapon(),
            .nextWeapon = shouldSwapToNextWeapon(),
            .lastWeapon = shouldSwapToLastWeapon(),
            .thrust = getThrust(),
            .sidewardThrust = getSidewardThrust(),
            .steer = getSteer(),
        };
    }

    virtual bool isShooting() const = 0;

    virtual bool shouldSwapToPreviousWeapon() const = 0;

    virtual bool shouldSwapToNextWeapon() const = 0;

    virtual bool shouldSwapToLastWeapon() const = 0;

    /// <returns>Value from interval <-1.f, 1.f></returns>
    virtual float getThrust() const = 0;

    /// <summary>
    /// Also known as strafing
    /// </summary>
    /// <returns>Value from interval <-1.f, 1.f></returns>
    virtual float getSidewardThrust() const = 0;

    /// <returns>Value from interval <-1.f, 1.f></returns>
    virtual float getSteer() const = 0;
};
