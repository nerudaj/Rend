#include "Server.hpp"
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

void Server::update(std::function<void(const std::string&)> log)
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
            if (!result.value().empty()) log(result.value());
            shouldUpdate = true;
        }
        else
        {
            log("error:" + result.error());
        }
    }

    if (!shouldUpdate) return;

    updateData.state = computeNewState();

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
            log(std::format(
                "error: Could not send response to client {} at {}:{}",
                client.idx,
                client.address.toInteger(),
                client.port));
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
    else if (updateData.state != ServerState::Lobby)
    {
        if (auto&& result = denyNewPeer(address, port); !result)
            return std::unexpected(result.error());
        return std::format(
            "Could not register new peer at {}:{}, game has started",
            address.toString(),
            port);
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
            return clients.size() - 1;
        }
        *itr = ClientData();
        return std::distance(clients.begin(), itr);
    }(updateData.clients);

    registeredClients[peerToId(address, port)] = ClientConnectionInfo {
        .idx = newIdx, .address = address, .port = port
    };

    auto&& packet =
        ServerMessage { .type = ServerMessageType::ConnectionAccepted,
                        .clientId = newIdx }
            .toPacket();

    if (socket->send(packet, address, port) != sf::Socket::Status::Done)
    {
        return std::unexpected(std::format(
            "Could not send ConnectionConfirmed to new peer at {}:{}",
            address.toString(),
            port));
    }

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
        "Map ready accepted from {}:{}", address.toString(), port);
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
            "Could not parse peer settings update json '{}', from client {}, "
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
    CHECK_REGISTRATION(address, port);

    auto&& id = peerToId(address, port);
    updateData.clients.at(registeredClients.at(id).idx).state =
        ClientState::Disconnected;
    registeredClients.erase(id);

    return std::format("Client {}:{} disconnected", address.toString(), port);
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
