#pragma once

#pragma warning(push, 0)
#include <DGM/dgm.hpp>
#include <SFML/Network.hpp>
#pragma warning(pop)
#include "ClientMessage.hpp"
#include "ServerDependencies.hpp"
#include "ServerEvents.hpp"
#include "ServerMessage.hpp"
#include <Error.hpp>
#include <Memory.hpp>
#include <expected>
#include <map>
#include <queue>
#include <string>

using LoadMapAsBase64 =
    std::function<std::string(const std::string&, const std::string&)>;

struct [[nodiscard]] ServerConfiguration final
{
    unsigned short port;
    unsigned short maxClientCount;
    bool acceptReconnects = false;
};

class [[nodiscard]] Server final
{
public:
    Server(ServerConfiguration config, ServerDependencies dependencies);
    ~Server();

public:
    void operator()(const PeerLeftServerEvent& e);
    void operator()(const MapRequestedServerEvent& e);

    void update();

private:
    using NewConnectionCount = size_t;
    using ProcessedPacketCount = size_t;

    NewConnectionCount processNewConnections();
    ProcessedPacketCount processIncomingPackets();
    void processEvents();
    void sendUpdates();

    ExpectedLog handleMessage(
        const ClientMessage& message,
        const sf::IpAddress& address,
        unsigned short port);

    ExpectedLog handleNewConnection(mem::Box<sf::TcpSocket>&& socket);

    ExpectedLog
    handlePeerReady(const sf::IpAddress& address, unsigned short port);

    ExpectedLog
    handleMapReady(const sf::IpAddress& address, unsigned short port);

    ExpectedLog
    handleMapEnded(const sf::IpAddress& address, unsigned short port);

    ExpectedLog handleReportedInput(const ClientMessage& message);

    ExpectedLog handleMapDownloadRequest(
        const sf::IpAddress& address,
        unsigned short port,
        const ClientMessage& message);

    ExpectedLog handlePeerSettingsUpdate(
        const ClientMessage& message,
        const sf::IpAddress& address,
        unsigned short port);

    ExpectedLog handleLobbySettingsUpdate(
        const ClientMessage& message,
        const sf::IpAddress& address,
        unsigned short port);

    ExpectedLog
    handleHeartbeat(const sf::IpAddress& address, unsigned short port);

    ExpectedLog
    handleDisconnection(const sf::IpAddress& address, unsigned short port);

    ExpectedLog denyNewPeer(
        mem::Box<sf::TcpSocket>&& socket, const std::string& denyReason);

    [[nodiscard]] bool
    isAdmin(const sf::IpAddress& address, unsigned short port) const noexcept
    {
        return isRegistered(address, port)
               && registeredClients.at(peerToId(address, port)).idx == 0u;
    }

    [[nodiscard]] bool isRegistered(
        const sf::IpAddress& address, unsigned short port) const noexcept
    {
        return registeredClients.contains(peerToId(address, port));
    }

    ServerState computeNewState();

    [[nodiscard]] constexpr auto getConnectedClientsOnly() const
    {
        return updateData.clients
               | std::views::filter(
                   [](const ClientData& client)
                   { return client.state != ClientState::Disconnected; });
    }

    [[nodiscard]] constexpr bool isMapLoadedForAllPeers() const
    {
        return std::ranges::all_of(
            getConnectedClientsOnly(),
            [](const ClientData& client)
            { return client.state == ClientState::ConnectedAndMapReady; });
    }

    [[nodiscard]] constexpr bool areAllPeersReady() const
    {
        return std::ranges::all_of(
            getConnectedClientsOnly(),
            [](const ClientData& client)
            { return client.state == ClientState::ConnectedAndReady; });
    }

    [[nodiscard]] constexpr bool isNoPeerReady() const
    {
        return std::ranges::all_of(
            getConnectedClientsOnly(),
            [](const ClientData& client)
            { return client.state == ClientState::Connected; });
    }

    [[nodiscard]] sf::Uint64 static peerToId(
        const sf::IpAddress& address, unsigned short port)
    {
        return sf::Uint64(address.toInteger()) + (sf::Uint64(port) << 32);
    }

    constexpr [[nodiscard]] ClientData&
    getClient(const sf::IpAddress& address, unsigned short port)
    {
        return updateData.clients.at(
            registeredClients.at(peerToId(address, port)).idx);
    }

private:
    struct ClientConnectionInfo
    {
        PlayerIdxType idx = 0;
        sf::IpAddress address;
        unsigned short port = 0;
        mem::Box<sf::TcpSocket> socket;
    };

    const unsigned short MAX_CLIENT_COUNT;
    const bool ACCEPT_RECONNECTS;
    ServerDependencies deps;
    mem::Box<sf::TcpListener> listener;
    std::map<sf::Uint64, ClientConnectionInfo> registeredClients;
    ServerUpdateData updateData;
    size_t sequence = 0;
    std::queue<ServerEvents> events;
};
