#pragma once

#include <DGM/dgm.hpp>
#include <input/ControllerInterface.hpp>
#include <input/InputSchema.hpp>

class SimpleController : public ControllerInterface
{
public:
    constexpr void
    deserializeFrom(dgm::UniversalReference<InputSchema> auto&& schema) noexcept
    {
        input = std::forward<decltype(schema)>(schema);
    }

    [[nodiscard]] constexpr bool isShooting() const override
    {
        return input.shooting;
    }

    [[nodiscard]] constexpr float getThrust() const override
    {
        return input.thrust;
    }

    [[nodiscard]] constexpr float getSidewardThrust() const override
    {
        return input.sidewardThrust;
    }

    [[nodiscard]] constexpr float getSteer() const override
    {
        return input.steer;
    }

private:
    InputSchema input;
};