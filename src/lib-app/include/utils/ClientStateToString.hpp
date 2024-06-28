#pragma once

#include <ClientData.hpp>
#include <Configs/Strings.hpp>

[[nodiscard]] constexpr static std::string
clientStateToString(ClientState state)
{
    switch (state)
    {
    case ClientState::Disconnected:
        return Strings::AppState::PeerLobby::DISCONNECTED;
    case ClientState::Connected:
        return Strings::AppState::PeerLobby::NOT_READY;
    case ClientState::ConnectedAndReady:
    case ClientState::ConnectedAndMapReady:
        return Strings::AppState::PeerLobby::READY;
    default:
        return "";
    }
}
