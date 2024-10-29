#pragma once

#include <compare>
#include <nlohmann/json.hpp>
#include <string>

struct [[nodiscard]] MapOptions final
{
    std::string name = "";
    bool enabled = false;

    [[nodiscard]] constexpr std::partial_ordering
    operator<=>(const MapOptions&) const = default;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapOptions, name, enabled);
