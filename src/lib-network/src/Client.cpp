#include "Client.hpp"
#include "ClientMessage.hpp"
#include "LobbySettings.hpp"
#include "MapDownloadRequest.hpp"
#include "ServerMessage.hpp"
#include "ServerMessageType.hpp"
#include <format>
#include <nlohmann/json.hpp>
#include <print>
#include <stdexcept>

Client::Client(const sf::IpAddress& address, unsigned short port)
    : remoteAddress(address), remotePort(port)
{
    auto&& id = registerToServer();
    if (!id) throw std::runtime_error(id.error());
    myClientId = *id;

    socket->setBlocking(false);
}

ExpectSuccess
Client::readIncomingPackets(HandleNetworkUpdate handleUpdateCallback)
{
    sf::Packet packet;

    while (socket->receive(packet) == sf::Socket::Status::Done)
    {
        auto&& message = ServerMessage::fromPacket(packet);
        if (message.type != ServerMessageType::Update) [[unlikely]]
        {
            return std::unexpected(std::format(
                "Expected server Update message, got type {}",
                std::to_underlying(message.type)));
        }

        handleUpdateCallback(nlohmann::json::parse(message.payload));
    }

    return ReturnFlag::Success;
}

ExpectSuccess Client::readIncomingPackets(
    HandleNetworkUpdate handleUpdateCallback,
    HandleMapDownload handleMapDownloadCallback)
{
    sf::Packet packet;

    while (socket->receive(packet) == sf::Socket::Status::Done)
    {
        auto&& message = ServerMessage::fromPacket(packet);

        if (message.type == ServerMessageType::Update) [[likely]]
        {
            handleUpdateCallback(nlohmann::json::parse(message.payload));
        }
        else if (message.type == ServerMessageType::MapDownloadResponse)
        {
            handleMapDownloadCallback(nlohmann::json::parse(message.payload));
        }
        else [[unlikely]]
        {
            return std::unexpected(std::format(
                "Expected server Update or MapDownload message, got type {}",
                std::to_underlying(message.type)));
        }
    }

    return ReturnFlag::Success;
}

ExpectSuccess Client::readPacketsUntil(
    HandleNetworkUpdate handleUpdateCallback,
    std::function<bool()> shouldStopReading,
    sf::Time timeout)
{
    sf::Clock clock;

    do
    {
        auto&& result = readIncomingPackets(handleUpdateCallback);
        if (!result) return result;

        if (clock.getElapsedTime() > timeout)
        {
            return std::unexpected("Packet reading timeouted");
        }
    } while (!shouldStopReading());

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

ExpectSuccess Client::requestMapDownload(
    const std::string& mapPackName, const std::vector<std::string>& mapNames)
{
    return trySendPacket(
        ClientMessage {
            .type = ClientMessageType::MapDownloadRequest,
            .clientId = myClientId,
            .jsonData =
                nlohmann::json(MapDownloadRequest { .mapPackName = mapPackName,
                                                    .mapNames = mapNames })
                    .dump() }
            .toPacket(),
        "Something went wrong when requesting map download");
}

std::expected<PlayerIdxType, ErrorMessage> Client::registerToServer()
{
    if (socket->connect(remoteAddress, remotePort, sf::seconds(3))
        != sf::Socket::Status::Done)
    {
        return std::unexpected("Cannot connect to remote host");
    }

    auto&& message = getConnectResponse();
    if (!message) return std::unexpected(message.error());
    return message->clientId;
}

std::expected<ServerMessage, ErrorMessage> Client::getConnectResponse()
{
    sf::Packet packet;

    if (socket->receive(packet) != sf::Socket::Status::Done)
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
