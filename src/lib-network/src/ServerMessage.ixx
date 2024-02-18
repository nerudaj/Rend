module;

#include <SFML/Network.hpp>
#include <string>

export module ServerMessage;

export import ServerMessageType;
export import NetworkTypes;

export struct [[nodiscard]] ServerMessage
{
    ServerMessageType type;
    PlayerIdType clientId;

    static ServerMessage fromPacket(sf::Packet& packet)
    {
        std::underlying_type_t<ServerMessageType> messageType;

        packet >> messageType;

        ServerMessage message;
        message.type = static_cast<ServerMessageType>(messageType);
        packet >> message.clientId;

        return message;
    }

    [[nodiscard]] sf::Packet toPacket() const
    {
        sf::Packet packet;
        packet << static_cast<std::underlying_type_t<ServerMessageType>>(type);
        packet << clientId;
        return packet;
    }
};
