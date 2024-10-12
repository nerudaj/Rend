#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>

enum class [[nodiscard]] Team : uint8_t
{
    None,
    Red,
    Blue
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    Team,
    {
        { Team::None, "none" },
        { Team::Red, "red" },
        { Team::Blue, "blue" },
    });
