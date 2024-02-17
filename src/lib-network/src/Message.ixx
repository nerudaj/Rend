module;

#include <SFML/Network.hpp>
#include <string>

export module Message;

export
{
    using PlayerIdType = std::uint8_t;
    using ChecksumType = std::uint64_t;

    enum class [[nodiscard]] ClientMessageType : uint8_t
    {
        ConnectionRequest,
        PeerSettingsUpdate,
        GameSettingsUpdate,
        CommitLobby,
        MapLoaded,
        ReportInput,
        Disconnect
    };

    struct [[nodiscard]] ClientMessage
    {
    };

    enum class [[nodiscard]] ServerMessageType : uint8_t
    {
        ConnectionAccepted,
        ConnectionRefused,
        LobbyCommited,
        StartGame,
        UpdateInput
    };

    struct [[nodiscard]] ServerMessage
    {
    };

    enum class MessageType : std::uint8_t
    {
        Connect,
        ConnectConfirmed,
        ConnectionRefused,
        PeerSettingsUpdate,
        GameSettingsUpdate,
        Update,
        Disconnect
    };

    struct [[nodiscard]] Message
    {
        MessageType type;
        PlayerIdType playerId;
        std::string playerName;
        std::string inputJson;
        std::size_t tick;
        ChecksumType checksum;

        static Message parseMessage(sf::Packet& packet);

        [[nodiscard]] sf::Packet toPacket() const;
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

module :private;

Message Message::parseMessage(sf::Packet& packet)
{
    auto&& result = Message();
    packet >> result;
    return result;
}

sf::Packet Message::toPacket() const
{
    auto&& packet = sf::Packet();
    packet << *this;
    return packet;
}
