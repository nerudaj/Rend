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
export import LobbySettings;

export struct PlayerConfig
{
    std::string name;
};

export using HandleNetworkUpdate = std::function<void(const ServerUpdateData&)>;

export class [[nodiscard]] Client final
{
public:
    Client(const sf::IpAddress& address, unsigned short port);

    ~Client()
    {
        disconnect();
    }

    ExpectSuccess readIncomingPackets(HandleNetworkUpdate handleUpdateCallback);

    ExpectSuccess sendMapReadySignal();

    ExpectSuccess commitLobby();

    ExpectSuccess
    sendUpdate(size_t tick, const std::vector<InputSchema>& inputs);

    ExpectSuccess sendLobbyUpdate(const LobbySettings& lobbySettings);

    ExpectSuccess sendMapEnded();

private:
    ExpectSuccess bindToAnyPort();

    std::expected<PlayerIdType, ErrorMessage> registerToServer();

    ExpectSuccess sendConnectPacket();

    std::expected<ServerMessage, ErrorMessage> getConnectResponse();

    ExpectSuccess
    trySendPacket(sf::Packet&& packet, const ErrorMessage& errorMessage)
    {
        if (socket->send(packet, remoteAddress, remotePort)
            != sf::Socket::Status::Done)
        {
            return std::unexpected(errorMessage);
        }
        return ReturnFlag::Success;
    }

    void disconnect();

private:
    sf::IpAddress remoteAddress;
    unsigned short remotePort;
    mem::Box<sf::UdpSocket> socket;
    unsigned short myPort;
    PlayerIdType myClientId;
};
