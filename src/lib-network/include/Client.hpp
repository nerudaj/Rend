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
    [[nodiscard]] PlayerIdxType getMyIndex() const noexcept
    {
        return myClientId;
    }

    ExpectSuccess readIncomingPackets(HandleNetworkUpdate handleUpdateCallback);

    ExpectSuccess readPacketsUntil(
        HandleNetworkUpdate handleUpdateCallback,
        std::function<bool()> shouldStopReading,
        sf::Time timeout = sf::seconds(2));

    ExpectSuccess sendMapReadySignal();

    ExpectSuccess sendPeerReadySignal();

    ExpectSuccess sendMapEndedSignal();

    ExpectSuccess
    sendCurrentFrameInputs(size_t tick, const std::vector<InputSchema>& inputs);

    ExpectSuccess sendLobbySettingsUpdate(const LobbySettings& lobbySettings);

    ExpectSuccess sendPeerSettingsUpdate(const ClientData& peerUpdate);

private:
    std::expected<PlayerIdxType, ErrorMessage> registerToServer();

    std::expected<ServerMessage, ErrorMessage> getConnectResponse();

    ExpectSuccess
    trySendPacket(sf::Packet&& packet, const ErrorMessage& errorMessage)
    {
        if (socket->send(packet) != sf::Socket::Status::Done)
        {
            return std::unexpected(errorMessage);
        }

        return ReturnFlag::Success;
    }

    void disconnect();

private:
    sf::IpAddress remoteAddress;
    unsigned short remotePort;
    mem::Box<sf::TcpSocket> socket;
    PlayerIdxType myClientId;
};
