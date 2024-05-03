#include "Client.hpp"
#include "ClientMessage.hpp"
#include "LobbySettings.hpp"
#include "ServerMessage.hpp"
#include "ServerMessageType.hpp"
#include <format>
#include <nlohmann/json.hpp>
#include <print>
#include <stdexcept>

Client::Client(const sf::IpAddress& address, unsigned short port)
    : remoteAddress(address), remotePort(port)
{
    if (auto&& result = bindToAnyPort(); !result)
        throw std::runtime_error(result.error());

    auto&& id = registerToServer();
    if (!id) throw std::runtime_error(id.error());
    myClientId = *id;

    socket->setBlocking(false);
}

ExpectSuccess
Client::readIncomingPackets(HandleNetworkUpdate handleUpdateCallback)
{
    sf::Packet packet;
    sf::IpAddress address;
    unsigned short port;

    while (socket->receive(packet, address, port) == sf::Socket::Status::Done)
    {
        auto&& message = ServerMessage::fromPacket(packet);
        if (message.type != ServerMessageType::Update)
        {
            return std::unexpected(std::format(
                "Expected server Update message, got type {}",
                std::to_underlying(message.type)));
        }

        handleUpdateCallback(nlohmann::json::parse(message.payload));
    }

    return ReturnFlag::Success;
}

ExpectSuccess Client::sendMapReadySignal()
{
    auto&& packet = ClientMessage { .type = ClientMessageType::MapLoaded,
                                    .clientId = myClientId }
                        .toPacket();
    if (socket->send(packet, remoteAddress, remotePort)
        != sf::Socket::Status::Done)
    {
        return std::unexpected(
            std::format("Could not send MapLoaded message to server"));
    }

    return ReturnFlag::Success;
}

ExpectSuccess Client::commitLobby()
{
    auto&& packet = ClientMessage { .type = ClientMessageType::CommitLobby,
                                    .clientId = myClientId }
                        .toPacket();
    if (socket->send(packet, remoteAddress, remotePort)
        != sf::Socket::Status::Done)
    {
        return std::unexpected(std::format("Could not commit lobby"));
    }

    return ReturnFlag::Success;
}

ExpectSuccess
Client::sendUpdate(size_t tick, const std::vector<InputSchema>& inputs)
{
    auto&& packet =
        ClientMessage { .type = ClientMessageType::ReportInput,
                        .clientId = myClientId,
                        .jsonData = nlohmann::json(inputs[myClientId]).dump(),
                        .tick = tick }
            .toPacket();
    if (socket->send(packet, remoteAddress, remotePort)
        != sf::Socket::Status::Done)
    {
        return std::unexpected(std::format("Could not send input update"));
    }

    return ReturnFlag::Success;
}

ExpectSuccess Client::sendLobbyUpdate(const LobbySettings& lobbySettings)
{
    return trySendPacket(
        ClientMessage { .type = ClientMessageType::GameSettingsUpdate,
                        .clientId = myClientId,
                        .jsonData = nlohmann::json(lobbySettings).dump() }
            .toPacket(),
        "Could not send lobby update");
}

ExpectSuccess Client::sendMapEnded()
{
    return trySendPacket(
        ClientMessage { .type = ClientMessageType::MapEnded,
                        .clientId = myClientId }
            .toPacket(),
        "Sending map ended failed");
}

ExpectSuccess Client::bindToAnyPort()
{
    if (socket->bind(sf::Socket::AnyPort) != sf::Socket::Status::Done)
    {
        return std::unexpected(std::format("Cannot bind socket to any port"));
    }

    myPort = socket->getLocalPort();
    std::println("Socket bound to port {}", myPort);

    return ReturnFlag::Success;
}

std::expected<PlayerIdType, ErrorMessage> Client::registerToServer()
{
    if (auto&& result = sendConnectPacket(); !result)
        return std::unexpected(result.error());

    auto&& message = getConnectResponse();
    if (!message) return std::unexpected(message.error());
    return message->clientId;
}

ExpectSuccess Client::sendConnectPacket()
{
    return trySendPacket(
        ClientMessage { .type = ClientMessageType::ConnectionRequest }
            .toPacket(),
        std::format(
            "Could not send message to {}:{}",
            remoteAddress.toString(),
            remotePort));
}

std::expected<ServerMessage, ErrorMessage> Client::getConnectResponse()
{
    sf::Packet packet;
    sf::IpAddress address;
    unsigned short port;

    if (socket->receive(packet, address, port) != sf::Socket::Status::Done)
    {
        return std::unexpected(
            std::format("Got no response from remote server"));
    }

    auto&& message = ServerMessage::fromPacket(packet);
    if (message.type != ServerMessageType::ConnectionAccepted)
    {
        return std::unexpected(std::format(
            "Expected ConnectConfirmed from the server, got {}",
            static_cast<std::underlying_type_t<ServerMessageType>>(
                message.type)));
    }

    return message;
}

void Client::disconnect()
{
    trySendPacket(
        ClientMessage { .type = ClientMessageType::Disconnect,
                        .clientId = myClientId }
            .toPacket(),
        "");
}
