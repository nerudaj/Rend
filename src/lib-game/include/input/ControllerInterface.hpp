#pragma once

class ControllerInterface
{
public:
    virtual ~ControllerInterface() = default;

public:
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
