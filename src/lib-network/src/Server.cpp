#include "Server.hpp"
#include "MapDownloadRequest.hpp"
#include "MapDownloadResponse.hpp"
#include <format>
#include <iostream>
#include <nlohmann/json.hpp>
#include <print>
#include <source_location>
#include <stdexcept>

#define CHECK_REGISTRATION(addr, port)                                         \
    if (!isRegistered(addr, port))                                             \
    {                                                                          \
        auto&& fnName =                                                        \
            std::string(std::source_location::current().function_name());      \
        return std::unexpected(std::format(                                    \
            "{}: Unauthorized attempt for access from {}:{}",                  \
            fnName,                                                            \
            addr.toString(),                                                   \
            port));                                                            \
    }

Server::Server(ServerConfiguration config, ServerDependencies dependencies)
    : MAX_CLIENT_COUNT(config.maxClientCount)
    , ACCEPT_RECONNECTS(config.acceptReconnects)
    , deps(dependencies)
{
    if (listener->listen(config.port) != sf::Socket::Status::Done)
    {
        throw std::runtime_error(
            std::format("Server: Cannot bind socket to port {}", config.port));
    }
    listener->setBlocking(false);

    deps.logger->log("Server: Listening on port {}", config.port);
}

Server::~Server()
{
    for (auto&& [id, client] : registeredClients)
    {
        client.socket->disconnect();
    }
}

void Server::operator()(const PeerLeftServerEvent& e)
{
    updateData.clients.at(registeredClients.at(e.clientKey).idx).state =
        ClientState::Disconnected;
    registeredClients.erase(e.clientKey);
}

void Server::operator()(const MapRequestedServerEvent& e)
{
    auto& client = registeredClients.at(e.clientKey);
    auto map = deps.mapLoader->loadMapInBase64(e.mapPackName, e.mapName);
    if (!map)
    {
        deps.logger->log(map);
        return;
    }

    auto packet =
        ServerMessage { .type = ServerMessageType::MapDownloadResponse,
                        .clientId = client.idx,
                        .payload = nlohmann::json(
                                       MapDownloadResponse {
                                           .mapPackName = e.mapPackName,
                                           .mapName = e.mapName,
                                           .base64encodedMap = map.value() })
                                       .dump() }
            .toPacket();

    if (client.socket->send(packet) != sf::Socket::Status::Done)
    {
        deps.logger->log(
            "error: Could not send map to client {} at "
            "{}:{}",
            client.idx,
            client.address.toInteger(),
            client.port);
    }
}

void Server::update()
{
    auto cnt1 = processNewConnections();
    auto cnt2 = processIncomingPackets();
    processEvents();
    if (cnt1 + cnt2 > 0) sendUpdates();
}

Server::NewConnectionCount Server::processNewConnections()
{
    size_t cnt = registeredClients.size();

    mem::Box<sf::TcpSocket> newConnection;
    if (listener->accept(*newConnection) == sf::Socket::Status::Done)
    {
        deps.logger->log(handleNewConnection(std::move(newConnection)));
    }

    return registeredClients.size() - cnt;
}

Server::ProcessedPacketCount Server::processIncomingPackets()
{
    size_t cnt = 0;

    sf::Packet packet;
    std::vector<sf::Uint64> clientKeysToRemove;
    for (auto&& [key, client] : registeredClients)
    {
        if (client.socket->receive(packet) != sf::Socket::Status::Done)
            continue;

        auto result = handleMessage(
            ClientMessage::fromPacket(packet), client.address, client.port);
        if (result) ++cnt;
        deps.logger->log(result);
    }

    return cnt;
}

void Server::processEvents()
{
    while (!events.empty())
    {
        std::visit(*this, events.front());
        events.pop();
    }
}

void Server::sendUpdates()
{
    updateData.state = computeNewState();

    auto&& payload = nlohmann::json(updateData).dump();
    for (auto&& [key, client] : registeredClients)
    {
        auto packet = ServerMessage { .type = ServerMessageType::Update,
                                      .sequence = sequence,
                                      .clientId = client.idx,
                                      .payload = payload }
                          .toPacket();

        if (auto status = client.socket->send(packet);
            status != sf::Socket::Status::Done)
        {
            deps.logger->log(
                "error: Could not send response to client {} at {}:{} (status "
                "code {})",
                client.idx,
                client.address.toInteger(),
                client.port,
                std::to_underlying(status));
        }
    }

    ++sequence;
    updateData.inputs.clear();
}

ExpectedLog Server::handleMessage(
    const ClientMessage& message,
    const sf::IpAddress& address,
    unsigned short port)
{
    switch (message.type)
    {
        using enum ClientMessageType;
    case PeerSettingsUpdate:
        return handlePeerSettingsUpdate(message, address, port);
    case LobbySettingsUpdate:
        return handleLobbySettingsUpdate(message, address, port);
    case ReportPeerReady:
        return handlePeerReady(address, port);
    case ReportMapReady:
        return handleMapReady(address, port);
    case ReportMapEnded:
        return handleMapEnded(address, port);
    case ReportInput:
        return handleReportedInput(message);
    case MapDownloadRequest:
        return handleMapDownloadRequest(address, port, message);
    case Disconnect:
        return handleDisconnection(address, port);
    default:
        return std::unexpected(std::format(
            "Unknown message {}",
            static_cast<std::underlying_type_t<ClientMessageType>>(
                message.type)));
    }
}

ExpectedLog Server::handleNewConnection(mem::Box<sf::TcpSocket>&& socket)
{
    auto&& address = socket->getRemoteAddress();
    auto&& port = socket->getRemotePort();

    if (isRegistered(address, port) && !ACCEPT_RECONNECTS)
    {
        return denyNewPeer(
            std::move(socket),
            std::format(
                "Could not register new peer at {}:{}, because it is already "
                "registered",
                address.toString(),
                port));
    }
    else if (MAX_CLIENT_COUNT == registeredClients.size())
    {
        return denyNewPeer(
            std::move(socket),
            std::format(
                "Could not register new peer at {}:{}, because limit of "
                "clients "
                "has been reached ",
                address.toString(),
                port));
    }
    else if (updateData.state != ServerState::Lobby)
    {
        return denyNewPeer(
            std::move(socket),
            std::format(
                "Could not register new peer at {}:{}, game has started",
                address.toString(),
                port));
    }

    const auto&& newIdx = [](std::vector<ClientData>& clients) -> PlayerIdxType
    {
        auto&& itr = std::ranges::find_if(
            clients,
            [](const ClientData& client)
            { return client.state == ClientState::Disconnected; });

        if (itr == clients.end())
        {
            clients.push_back(ClientData());
            return static_cast<PlayerIdxType>(clients.size() - 1);
        }
        *itr = ClientData();
        return static_cast<PlayerIdxType>(std::distance(clients.begin(), itr));
    }(updateData.clients);

    auto&& packet =
        ServerMessage { .type = ServerMessageType::ConnectionAccepted,
                        .clientId = newIdx }
            .toPacket();

    if (socket->send(packet) != sf::Socket::Status::Done)
    {
        return std::unexpected(std::format(
            "Could not send ConnectionConfirmed to new peer at {}:{}",
            address.toString(),
            port));
    }

    socket->setBlocking(false);
    registeredClients[peerToId(address, port)] =
        ClientConnectionInfo { .idx = newIdx,
                               .address = address,
                               .port = port,
                               .socket = std::move(socket) };

    return std::format(
        "Registered new client at {}:{} with ID: {}",
        address.toString(),
        port,
        newIdx);
}

ExpectedLog
Server::handlePeerReady(const sf::IpAddress& address, unsigned short port)
{
    CHECK_REGISTRATION(address, port);

    getClient(address, port).state = ClientState::ConnectedAndReady;

    return std::format(
        "Peer ready accepted from {}:{}", address.toString(), port);
}

ExpectedLog
Server::handleMapReady(const sf::IpAddress& address, unsigned short port)
{
    CHECK_REGISTRATION(address, port);

    getClient(address, port).state = ClientState::ConnectedAndMapReady;

    return std::format(
        "Map ready accepted from {}:{}", address.toString(), port);
}

ExpectedLog
Server::handleMapEnded(const sf::IpAddress& address, unsigned short port)
{
    CHECK_REGISTRATION(address, port);

    getClient(address, port).state = ClientState::Connected;

    return std::format(
        "Map ended accepted from {}:{}", address.toString(), port);
}

ExpectedLog Server::handleReportedInput(const ClientMessage& message)
{
    try
    {
        updateData.inputs.push_back(
            InputData { .clientId = message.clientId,
                        .tick = message.tick,
                        .input = nlohmann::json::parse(message.jsonData) });
    }
    catch (const std::exception& e)
    {
        return std::unexpected(std::format(
            "Could not parse incoming input json '{}', from client {}, "
            "with "
            "error: {}",
            message.jsonData,
            message.clientId,
            e.what()));
    }

#ifdef _DEBUG
    return std::format(
        "Processed input from client {}, tick {}, json {}",
        message.clientId,
        message.tick,
        message.jsonData);
#else
    return std::format("");
#endif
}

ExpectedLog Server::handleMapDownloadRequest(
    const sf::IpAddress& address,
    unsigned short port,
    const ClientMessage& message)
{
    CHECK_REGISTRATION(address, port);
    auto&& id = peerToId(address, port);

    try
    {
        MapDownloadRequest request = nlohmann::json::parse(message.jsonData);

        for (const auto& mapName : request.mapNames)
        {
            events.push(
                MapRequestedServerEvent { .clientKey = id,
                                          .mapPackName = request.mapPackName,
                                          .mapName = mapName });
        }
    }
    catch (const std::exception& e)
    {
        return std::unexpected(std::format(
            "Could not parse incoming input json '{}', from client {}, "
            "with "
            "error: {}",
            message.jsonData,
            message.clientId,
            e.what()));
    }

    return std::format(
        "Map request was accepted from client {} with this payload: '{}'",
        message.clientId,
        message.jsonData);
}

ExpectedLog Server::handlePeerSettingsUpdate(
    const ClientMessage& message,
    const sf::IpAddress& address,
    unsigned short port)
{
    CHECK_REGISTRATION(address, port);

    try
    {
        auto&& data = ClientData(nlohmann::json::parse(message.jsonData));
        getClient(address, port).name = data.name;
        getClient(address, port).hasAutoswapOnPickup = data.hasAutoswapOnPickup;
    }
    catch (const std::exception& e)
    {
        return std::unexpected(std::format(
            "Could not parse peer settings update json '{}', from client "
            "{}, "
            "with error: {}",
            message.jsonData,
            message.clientId,
            e.what()));
    }

    return std::format(
        "Processed peer update from client {}, json {}",
        message.clientId,
        message.jsonData);
}

ExpectedLog Server::handleLobbySettingsUpdate(
    const ClientMessage& message,
    const sf::IpAddress& address,
    unsigned short port)
{
    if (!isAdmin(address, port))
        return std::unexpected(std::format(
            "handleLobbySettingsUpdate: Unauthorized attempt for access "
            "from "
            "{}:{}",
            address.toString(),
            port));

    try
    {
        updateData.lobbySettings = nlohmann::json::parse(message.jsonData);
    }
    catch (const std::exception& e)
    {
        return std::unexpected(std::format(
            "Could not parse incoming lobby settings update json '{}', "
            "from "
            "client {}, with "
            "error: {}",
            message.jsonData,
            message.clientId,
            e.what()));
    }

    return std::format(
        "Processed lobby update from client {}, json {}",
        message.clientId,
        message.jsonData);
}

ExpectedLog
Server::handleDisconnection(const sf::IpAddress& address, unsigned short port)
{
    CHECK_REGISTRATION(address, port);

    auto&& id = peerToId(address, port);
    events.push(PeerLeftServerEvent { .clientKey = id });

    return std::format("Client {}:{} disconnected", address.toString(), port);
}

ExpectedLog Server::denyNewPeer(
    mem::Box<sf::TcpSocket>&& socket, const std::string& denyReason)
{
    auto&& packet =
        ServerMessage { .type = ServerMessageType::ConnectionRefused }
            .toPacket();
    if (socket->send(packet) != sf::Socket::Status::Done)
    {
        return std::unexpected(std::format(
            "Failed to send ConnectionRefused to remote peer at {}:{}",
            socket->getRemoteAddress().toString(),
            socket->getRemotePort()));
    }

    return denyReason;
}

ServerState Server::computeNewState()
{
    if (getConnectedClientsOnly().empty())
    {
        return ServerState::Lobby;
    }

    if (updateData.state == ServerState::Lobby && areAllPeersReady())
    {
        return ServerState::MapLoading;
    }
    else if (
        updateData.state == ServerState::MapLoading && isMapLoadedForAllPeers())
    {
        return ServerState::GameInProgress;
    }
    else if (updateData.state == ServerState::GameInProgress && isNoPeerReady())
    {
        return ServerState::Lobby;
    }

    return updateData.state;
}
