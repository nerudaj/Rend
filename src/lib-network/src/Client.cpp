#include <format>
#include <print>
#include <stdexcept>

import Client;

Client::Client(const sf::IpAddress& address, unsigned short port)
    : remoteAddress(address), remotePort(port)
{
    if (auto&& result = bindToAnyPort(); !result)
        throw std::runtime_error(result.error());

    auto&& id = registerToServer();
    if (!id) throw std::runtime_error(id.error());
    myClientId = *id;
}

void Client::readIncomingPackets(
    UpdatePlayerConfigCallback, UpdatePlayerInputCallback)
{
    sf::Packet packet;
    sf::IpAddress address;
    unsigned short port;

    while (socket->receive(packet, address, port) == sf::Socket::Status::Done)
    {
        auto&& result =
            handleIncomingMessage(ServerMessage::fromPacket(packet));
    }
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
    auto&& packet =
        ClientMessage { .type = ClientMessageType::ConnectionRequest }
            .toPacket();
    if (socket->send(packet, remoteAddress, remotePort)
        != sf::Socket::Status::Done)
    {
        return std::unexpected(std::format(
            "Could not send message to {}:{}",
            remoteAddress.toString(),
            remotePort));
    }

    return ReturnFlag::Success;
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

ExpectSuccess Client::handleIncomingMessage(const ServerMessage& message)
{
    switch (message.type)
    {
        using enum ServerMessageType;

    case ConnectionAccepted:
    case ConnectionRefused:
    case LobbyCommited:
    case StartGame:
    case UpdateInput:
        break;
    default:
        return std::unexpected(std::format(
            "Got invalid message code {}",
            static_cast<std::underlying_type_t<ServerMessageType>>(
                message.type)));
    }

    return ReturnFlag::Success;
}