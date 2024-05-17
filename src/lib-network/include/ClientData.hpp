#pragma once

#include <nlohmann/json.hpp>
#include <string>

enum class [[nodiscard]] ClientState
{
    Disconnected,
    Connected,
    ConnectedAndReady,
    ConnectedAndMapReady
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    ClientState,
    {
        { ClientState::Disconnected, "disconnected" },
        { ClientState::Connected, "connected" },
        { ClientState::ConnectedAndReady, "connectedAndReady" },
        { ClientState::ConnectedAndMapReady, "connectedAndMapReady" },
    });

struct [[nodiscard]] ClientData final
{
    ClientState state = ClientState::Connected;
    std::string name = "player";
    bool hasAutoswapOnPickup = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    ClientData, state, name, hasAutoswapOnPickup);
