#pragma once

#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include "ServerMessageType.hpp"
#include "ClientData.hpp"
#include "LobbySettings.hpp"
#include "InputData.hpp"


struct [[nodiscard]] ServerUpdateData final
{
	bool lobbyCommited = false;
	bool peersReady = false;
	LobbySettings lobbySettings;
	std::vector<ClientData> clients;
	std::vector<InputData> inputs;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	ServerUpdateData,
	lobbyCommited,
	peersReady,
	lobbySettings,
	clients,
	inputs);

struct [[nodiscard]] ServerMessage final
{
    ServerMessageType type;
    size_t sequence = 0;
    PlayerIdType clientId;
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
