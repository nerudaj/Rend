module;

#include <SFML/Network.hpp>
#include <string>

export module Message;

export
{
    using PlayerIdType = std::uint8_t;
    using ChecksumType = std::uint64_t;

    enum class MessageType : std::uint8_t
    {
        Connect,
        Update,
        Disconnect
    };

    struct Message
    {
        PlayerIdType playerId;
        MessageType type;
        std::string playerName;
        std::string inputJson;
        std::size_t tick;
        ChecksumType checksum;
    };

    sf::Packet& operator<<(sf::Packet& packet, const Message& message)
    {
        return packet << message.playerId
                      << static_cast<std::underlying_type_t<MessageType>>(
                             message.type)
                      << message.playerName << message.inputJson << message.tick
                      << message.checksum;
    }

    sf::Packet& operator>>(sf::Packet& packet, Message& message)
    {
        std::underlying_type_t<MessageType> type;
        packet >> message.playerId >> type >> message.playerName
            >> message.inputJson >> message.tick >> message.checksum;
        message.type = static_cast<MessageType>(type);
        return packet;
    }
}