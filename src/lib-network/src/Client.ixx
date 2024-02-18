module;

#pragma warning(push, 0)
#include <SFML/Network.hpp>
#pragma warning(pop)
#include <functional>

export module Client;

export import ClientMessage;
export import ServerMessage;
export import Error;
export import Memory;

export struct PlayerConfig
{
    std::string name;
};

export using UpdatePlayerConfigCallback =
    std::function<void(PlayerIdType, PlayerConfig)>;
export using UpdatePlayerInputCallback =
    std::function<void(PlayerIdType, std::string)>;

export class [[nodiscard]] Client final
{
public:
    Client(const sf::IpAddress& address, unsigned short port);

    void readIncomingPackets(
        UpdatePlayerConfigCallback updatePlayerConfigCallback,
        UpdatePlayerInputCallback updatePlayerInputCallback);

    ExpectSuccess sendMapReadySignal();

private:
    ExpectSuccess bindToAnyPort();

    std::expected<PlayerIdType, ErrorMessage> registerToServer();

    ExpectSuccess sendConnectPacket();

    std::expected<ServerMessage, ErrorMessage> getConnectResponse();

    ExpectSuccess handleIncomingMessage(const ServerMessage& message);

private:
    sf::IpAddress remoteAddress;
    unsigned short remotePort;
    mem::Box<sf::UdpSocket> socket;
    unsigned short myPort;
    PlayerIdType myClientId;
};
