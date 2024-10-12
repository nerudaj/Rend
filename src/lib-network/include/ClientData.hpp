#pragma once

#include "UserOptions.hpp"
#include "enums/Team.hpp"
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
    UserOptions userOpts;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ClientData, state, userOpts);
