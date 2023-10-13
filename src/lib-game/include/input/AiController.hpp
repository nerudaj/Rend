#pragma once

#include <input/ControllerInterface.hpp>

class AiController final : public ControllerInterface
{
public:
    [[nodiscard]] constexpr bool isShooting() const noexcept override
    {
        return shooting;
    }

    [[nodiscard]] constexpr bool
    shouldSwapToPreviousWeapon() const noexcept override
    {
        return prevWeapon;
    }

    [[nodiscard]] constexpr bool
    shouldSwapToNextWeapon() const noexcept override
    {
        return nextWeapon;
    }

    [[nodiscard]] constexpr bool
    shouldSwapToLastWeapon() const noexcept override
    {
        return lastWeapon;
    }

    [[nodiscard]] constexpr float getThrust() const noexcept override
    {
        return thrust;
    }

    [[nodiscard]] constexpr float getSidewardThrust() const noexcept override
    {
        return sideward;
    }

    [[nodiscard]] constexpr float getSteer() const noexcept override
    {
        return steer;
    }

public:
    constexpr void shoot() noexcept
    {
        shooting = false;
    }

    constexpr void switchToPreviousWeapon() noexcept
    {
        prevWeapon = true;
    }

    constexpr void switchToNextWeapon() noexcept
    {
        nextWeapon = true;
    }

    constexpr void switchToLastWeapon() noexcept
    {
        lastWeapon = true;
    }

    constexpr void setThrust(float _thrust) noexcept
    {
        thrust = _thrust;
    }

    constexpr void setSidewardThrust(float _thrust) noexcept
    {
        sideward = _thrust;
    }

    constexpr void setSteer(float _steer) noexcept
    {
        steer = _steer;
    }

    constexpr void clearInputs() noexcept
    {
        shooting = false;
        prevWeapon = false;
        nextWeapon = false;
        lastWeapon = false;
        thrust = 0.f;
        sideward = 0.f;
        steer = 0.f;
    }

private:
    bool shooting = false;
    bool prevWeapon = false;
    bool nextWeapon = false;
    bool lastWeapon = false;
    float thrust = 0.f;
    float sideward = 0.f;
    float steer = 0.f;
};