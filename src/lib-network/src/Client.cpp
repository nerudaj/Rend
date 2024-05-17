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
    return trySendPacket(
        ClientMessage { .type = ClientMessageType::ReportMapReady,
                        .clientId = myClientId }
            .toPacket(),
        "Could not send map ready signal");
}

ExpectSuccess Client::sendPeerReadySignal()
{
    return trySendPacket(
        ClientMessage { .type = ClientMessageType::ReportPeerReady,
                        .clientId = myClientId }
            .toPacket(),
        "Could not send peer ready signal");
}

ExpectSuccess Client::sendMapEndedSignal()
{
    return trySendPacket(
        ClientMessage { .type = ClientMessageType::ReportMapEnded,
                        .clientId = myClientId }
            .toPacket(),
        "Sending map ended failed");
}

ExpectSuccess Client::sendCurrentFrameInputs(
    size_t tick, const std::vector<InputSchema>& inputs)
{
    return trySendPacket(
        ClientMessage { .type = ClientMessageType::ReportInput,
                        .clientId = myClientId,
                        .jsonData = nlohmann::json(inputs[myClientId]).dump(),
                        .tick = tick }
            .toPacket(),
        "Could not send input update");
}

ExpectSuccess
Client::sendLobbySettingsUpdate(const LobbySettings& lobbySettings)
{
    return trySendPacket(
        ClientMessage { .type = ClientMessageType::LobbySettingsUpdate,
                        .clientId = myClientId,
                        .jsonData = nlohmann::json(lobbySettings).dump() }
            .toPacket(),
        "Could not send lobby update");
}

ExpectSuccess Client::sendPeerSettingsUpdate(const ClientData& peerUpdate)
{
    return trySendPacket(
        ClientMessage { .type = ClientMessageType::PeerSettingsUpdate,
                        .clientId = myClientId,
                        .jsonData = nlohmann::json(peerUpdate).dump() }
            .toPacket(),
        "Could not send peer update");
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

std::expected<PlayerIdxType, ErrorMessage> Client::registerToServer()
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
