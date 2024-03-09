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

export using HandleNetworkUpdate = std::function<void(const ServerUpdateData&)>;

export class [[nodiscard]] Client final
{
public:
    Client(const sf::IpAddress& address, unsigned short port);

    ExpectSuccess readIncomingPackets(HandleNetworkUpdate handleUpdateCallback);

    ExpectSuccess sendMapReadySignal();

    ExpectSuccess commitLobby();

    ExpectSuccess
    sendUpdate(size_t tick, const std::vector<InputSchema>& inputs);

private:
    ExpectSuccess bindToAnyPort();

    std::expected<PlayerIdType, ErrorMessage> registerToServer();

    ExpectSuccess sendConnectPacket();

    std::expected<ServerMessage, ErrorMessage> getConnectResponse();

private:
    sf::IpAddress remoteAddress;
    unsigned short remotePort;
    mem::Box<sf::UdpSocket> socket;
    unsigned short myPort;
    PlayerIdType myClientId;
};
