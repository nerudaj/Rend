#pragma once

#include <SFML/Network.hpp>
#include <string>

#include "ClientMessageType.hpp"

struct [[nodiscard]] ClientMessage final
{
    ClientMessageType type;
    PlayerIdType clientId;
    std::string jsonData; // either map configuration JSON or input JSON
    size_t tick;

    static ClientMessage fromPacket(sf::Packet& packet)
    {
        std::underlying_type_t<ClientMessageType> messageType;

        packet >> messageType;

        ClientMessage message;
        message.type = static_cast<ClientMessageType>(messageType);
        packet >> message.clientId;
        packet >> message.jsonData;
        packet >> message.tick;

        return message;
    }

    [[nodiscard]] sf::Packet toPacket() const
    {
        sf::Packet packet;
        packet << static_cast<std::underlying_type_t<ClientMessageType>>(type);
        packet << clientId;
        packet << jsonData;
        packet << tick;
        return packet;
    }
};
