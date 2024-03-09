module;

#include <SFML/Network.hpp>
#include <string>

export module ClientMessage;

export import ClientMessageType;
export import NetworkTypes;

export struct [[nodiscard]] ClientMessage
{
    ClientMessageType type;
    PlayerIdType clientId;
    std::string clientName;
    std::string jsonData; // either map configuration JSON or input JSON
    size_t tick;

    static ClientMessage fromPacket(sf::Packet& packet)
    {
        std::underlying_type_t<ClientMessageType> messageType;

        packet >> messageType;

        ClientMessage message;
        message.type = static_cast<ClientMessageType>(messageType);
        packet >> message.clientId;

        return message;
    }

    [[nodiscard]] sf::Packet toPacket() const
    {
        sf::Packet packet;
        packet << static_cast<std::underlying_type_t<ClientMessageType>>(type);
        packet << clientId;
        return packet;
    }
};
