#pragma once

#pragma warning(push, 0)
#include <SFML/Network.hpp>
#pragma warning(pop)
#include "ServerMessage.hpp"
#include <Error.hpp>
#include <Memory.hpp>
#include <functional>

using HandleNetworkUpdate = std::function<void(const ServerUpdateData&)>;

class [[nodiscard]] Client final
{
public:
    Client(const sf::IpAddress& address, unsigned short port);

    ~Client()
    {
        disconnect();
    }

public:
    ExpectSuccess readIncomingPackets(HandleNetworkUpdate handleUpdateCallback);

    ExpectSuccess sendMapReadySignal();

    ExpectSuccess sendPeerReadySignal();

    ExpectSuccess sendMapEndedSignal();

    ExpectSuccess
    sendCurrentFrameInputs(size_t tick, const std::vector<InputSchema>& inputs);

    ExpectSuccess sendLobbySettingsUpdate(const LobbySettings& lobbySettings);

    ExpectSuccess sendPeerSettingsUpdate(const ClientData& peerUpdate);

private:
    ExpectSuccess bindToAnyPort();

    std::expected<PlayerIdxType, ErrorMessage> registerToServer();

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
    PlayerIdxType myClientId;
};
