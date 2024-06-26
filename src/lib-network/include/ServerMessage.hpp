#pragma once

#include "ClientData.hpp"
#include "InputData.hpp"
#include "LobbySettings.hpp"
#include "ServerMessageType.hpp"
#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>
#include <string>

enum class [[nodiscard]] ServerState
{
    Lobby,
    MapLoading,
    GameInProgress
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    ServerState,
    { { ServerState::Lobby, "lobby" },
      { ServerState::MapLoading, "mapLoading" },
      { ServerState::GameInProgress, "gameInProgress" } });

struct [[nodiscard]] ServerUpdateData final
{
    ServerState state = ServerState::Lobby;
    LobbySettings lobbySettings;
    std::vector<ClientData> clients;
    std::vector<InputData> inputs;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    ServerUpdateData, state, lobbySettings, clients, inputs);

struct [[nodiscard]] ServerMessage final
{
    ServerMessageType type;
    size_t sequence = 0;
    PlayerIdxType clientId; // TODO: remove?
    std::string payload;

    static ServerMessage fromPacket(sf::Packet& packet)
    {
        std::underlying_type_t<ServerMessageType> messageType;

        packet >> messageType;

        ServerMessage message;
        message.type = static_cast<ServerMessageType>(messageType);
        packet >> message.sequence;
        packet >> message.clientId;
        packet >> message.payload;

        return message;
    }

    [[nodiscard]] sf::Packet toPacket() const
    {
        sf::Packet packet;
        packet << static_cast<std::underlying_type_t<ServerMessageType>>(type);
        packet << sequence;
        packet << clientId;
        packet << payload;
        return packet;
    }
};
