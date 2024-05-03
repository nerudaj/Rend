#pragma once

#include <algorithm>
#include <core/Constants.hpp>
#include <string>

class [[nodiscard]] HudMessage final
{
public:
    HudMessage() = default;

    constexpr HudMessage(const std::string& message)
        : message(message), timeout(HUD_MESSAGE_DURATION)
    {
    }

public:
    constexpr void update(const float deltaTime) noexcept
    {
        timeout = std::clamp(timeout - deltaTime, 0.f, HUD_MESSAGE_DURATION);
    }

    [[nodiscard]] constexpr const std::string& getText() const noexcept
    {
        return message;
    }

    [[nodiscard]] constexpr bool isActive() const noexcept
    {
        return timeout > 0.f;
    }

private:
    std::string message;
    float timeout = 0.f;
};