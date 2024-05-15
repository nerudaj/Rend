#include "Server.hpp"
#include <format>
#include <iostream>
#include <nlohmann/json.hpp>
#include <print>
#include <stdexcept>

Server::Server(ServerConfiguration config)
    : MAX_CLIENT_COUNT(config.maxClientCount)
    , ACCEPT_RECONNECTS(config.acceptReconnects)
{
    if (socket->bind(config.port) != sf::Socket::Status::Done)
    {
        throw std::runtime_error(
            std::format("Server: Cannot bind socket to port {}", config.port));
    }

    std::println("Server: Listening on port {}", config.port);
}

void Server::update()
{
    socket->setBlocking(false);
    sf::IpAddress remoteAddress;
    unsigned short remotePort;
    sf::Packet packet;

    bool shouldUpdate = false;
    while (socket->receive(packet, remoteAddress, remotePort)
           == sf::Socket::Status::Done)
    {
        auto&& result = handleMessage(
            ClientMessage::fromPacket(packet), remoteAddress, remotePort);

        if (result)
        {
            if (!result.value().empty())
                std::cout << "Server: " << result.value() << std::endl;
            shouldUpdate = true;
        }
        else
        {
            std::cout << "Server:error: " << result.error() << std::endl;
        }
    }

    if (!shouldUpdate) return;

    auto&& payload = nlohmann::json(updateData).dump();
    for (auto&& [key, client] : registeredClients)
    {
        packet = ServerMessage { .type = ServerMessageType::Update,
                                 .sequence = sequence,
                                 .clientId = client.idx,
                                 .payload = payload }
                     .toPacket();

        if (socket->send(packet, client.address, client.port)
            != sf::Socket::Status::Done)
        {
            std::cout << "Server:error "
                      << std::format(
                             "Could not send response to client {} at {}:{}",
                             client.idx,
                             client.address.toInteger(),
                             client.port)
                      << std::endl;
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
    case ConnectionRequest:
        return handleNewConnection(address, port);
    case PeerSettingsUpdate:
        return handlePeerSettingsUpdate(message, address, port);
    case GameSettingsUpdate:
        return handleLobbySettingsUpdate(message, address, port);
    case CommitLobby:
        return handleLobbyCommited(address, port);
    case MapLoaded:
        return handleMapReady(address, port);
    case ReportInput:
        return handleReportedInput(message);
    case MapEnded:
        return handleMapEnded(address, port);
    case Disconnect:
        return handleDisconnection(address, port);
    default:
        return std::unexpected(std::format(
            "Unknown message {}",
            static_cast<std::underlying_type_t<ClientMessageType>>(
                message.type)));
    }
}

ExpectedLog
Server::handleNewConnection(const sf::IpAddress& address, unsigned short port)
{
    if (isRegistered(address, port) && !ACCEPT_RECONNECTS)
    {
        if (auto&& result = denyNewPeer(address, port); !result)
            return std::unexpected(result.error());
        return std::format(
            "Could not register new peer at {}:{}, because it is already "
            "registered",
            address.toString(),
            port);
    }
    else if (MAX_CLIENT_COUNT == registeredClients.size())
    {
        if (auto&& result = denyNewPeer(address, port); !result)
            return std::unexpected(result.error());
        return std::format(
            "Could not register new peer at {}:{}, because limit of clients "
            "has been reached ",
            address.toString(),
            port);
    }
    else if (updateData.lobbyCommited)
    {
        if (auto&& result = denyNewPeer(address, port); !result)
            return std::unexpected(result.error());
        return std::format(
            "Could not register new peer at {}:{}, game has started",
            address.toString(),
            port);
    }

    auto&& newId = static_cast<PlayerIdType>(registeredClients.size());
    auto&& packet =
        ServerMessage { .type = ServerMessageType::ConnectionAccepted,
                        .clientId = newId }
            .toPacket();

    if (socket->send(packet, address, port) != sf::Socket::Status::Done)
    {
        return std::unexpected(std::format(
            "Could not send ConnectionConfirmed to new peer at {}:{}",
            address.toString(),
            port));
    }

    registeredClients[peerToId(address, port)] =
        ClientConnectionInfo { .idx = newId, .address = address, .port = port };
    updateData.clients.push_back(ClientData {});

    return std::format(
        "Registered new client at {}:{} with ID: {}",
        address.toString(),
        port,
        newId);
}

ExpectedLog
Server::handleLobbyCommited(const sf::IpAddress& address, unsigned short port)
{
    if (!isAdmin(address, port))
        return std::unexpected(std::format(
            "handleLobbyCommited: Unauthorized attempt for access from {}:{}",
            address.toString(),
            port));

    updateData.lobbyCommited = true;

    for (auto&& [key, client] : registeredClients)
    {
        client.isMapReady = false;
    }

    return "Lobby was commited";
}

ExpectedLog
Server::handleMapReady(const sf::IpAddress& address, unsigned short port)
{
    if (!isRegistered(address, port))
    {
        return std::unexpected(std::format(
            "handleMapReady: Unauthorized attempt for access from {}:{}",
            address.toString(),
            port));
    }

    registeredClients.at(peerToId(address, port)).isMapReady = true;

    updateData.mapReady = isMapLoadedForAllPeers();

    return std::format(
        "Map ready accepted from {}:{}, all peers ready? {}",
        address.toString(),
        port,
        updateData.peersReady);
}

ExpectedLog
Server::handleMapEnded(const sf::IpAddress& address, unsigned short port)
{
    if (!isRegistered(address, port))
        return std::unexpected(std::format(
            "handleMapEnded: Unauthorized attempt for access from {}:{}",
            address.toString(),
            port));

    auto&& registeredClient = registeredClients.at(peerToId(address, port));
    registeredClient.isMapReady = false;
    registeredClient.isReady = false;
    updateData.clients[registeredClient.idx].isReady = false;
    updateGlobalReadyStatuses();

    return std::format(
        "Client {}:{} send map ended. Game active? {}",
        address.toString(),
        port,
        updateData.lobbyCommited);
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
            "Could not parse incoming input json '{}', from client {}, with "
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

ExpectedLog Server::handlePeerSettingsUpdate(
    const ClientMessage& message,
    const sf::IpAddress& address,
    unsigned short port)
{
    if (!isRegistered(address, port))
    {
        return std::unexpected(std::format(
            "handlePeerSettingsUpdate: Unauthorized attempt for access from "
            "{}:{}",
            address.toString(),
            port));
    }

    try
    {
        updateData.clients[message.clientId] =
            nlohmann::json::parse(message.jsonData);
        registeredClients.at(peerToId(address, port)).isReady =
            updateData.clients[message.clientId].isReady;
    }
    catch (const std::exception& e)
    {
        return std::unexpected(std::format(
            "Could not parse peer settings update json '{}', from client {}, "
            "with error: {}",
            message.jsonData,
            message.clientId,
            e.what()));
    }

    updateGlobalReadyStatuses();

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
            "handleLobbySettingsUpdate: Unauthorized attempt for access from "
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
            "Could not parse incoming lobby settings update json '{}', from "
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
    if (!isRegistered(address, port))
        return std::unexpected(std::format(
            "handleDisconnection: Unauthorized attempt for access from {}:{}",
            address.toString(),
            port));

    auto&& id = peerToId(address, port);
    updateData.clients.at(registeredClients.at(id).idx).active = false;
    registeredClients.erase(id);

    return std::format(
        "Client {}:{} disconnected. Game active? {}",
        address.toString(),
        port,
        updateData.lobbyCommited);
}

ExpectSuccess
Server::denyNewPeer(const sf::IpAddress& address, unsigned short port)
{
    auto&& packet =
        ServerMessage { .type = ServerMessageType::ConnectionRefused }
            .toPacket();
    if (socket->send(packet, address, port) != sf::Socket::Status::Done)
    {
        return std::unexpected(std::format(
            "Failed to send ConnectionRefused to remote peer at {}:{}",
            address.toString(),
            port));
    }

    return ReturnFlag::Success;
}

void Server::updateGlobalReadyStatuses()
{
    updateData.mapReady = isMapLoadedForAllPeers();
    updateData.peersReady = areAllPeersReady();
    updateData.lobbyCommited = !isNoPeerReady();
}
