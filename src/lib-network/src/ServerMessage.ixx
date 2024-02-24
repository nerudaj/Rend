module;

#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>
#include <string>

export module ServerMessage;

export import ServerMessageType;
export import NetworkTypes;
export import ClientData;
export import MapData;
export import InputData;

export
{
    struct [[nodiscard]] ServerUpdateData
    {
        bool lobbyCommited = false;
        bool peersReady = false;
        MapData mapinfo;
        std::vector<ClientData> clients;
        std::vector<InputData> inputs;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        ServerUpdateData, lobbyCommited, peersReady, mapinfo, clients, inputs);
}

export struct [[nodiscard]] ServerMessage
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
