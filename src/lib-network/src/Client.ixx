module;

#pragma warning(push, 0)
#include <SFML/Network.hpp>
#pragma warning(pop)
#include <cassert>
#include <concepts>
#include <expected>
#include <format>
#include <functional>
#include <print>
#include <stdexcept>
#include <string>

export module Client;

import ClientMessage;
import ServerMessage;
import Error;
import Memory;

export struct PlayerConfig
{
    std::string name;
};

export using UpdatePlayerConfigCallback =
    std::function<void(PlayerIdType, PlayerConfig)>;
export using UpdatePlayerInputCallback =
    std::function<void(PlayerIdType, std::string)>;

export class [[nodiscard]] Client
{
public:
    Client(
        const sf::IpAddress& address,
        unsigned short port,
        UpdatePlayerConfigCallback updatePlayerConfigCallback,
        UpdatePlayerInputCallback updatePlayerInputCallback)
        : remoteAddress(address)
        , remotePort(port)
        , updatePlayerConfigCallback(updatePlayerConfigCallback)
        , updatePlayerInputCallback(updatePlayerInputCallback)
    {
    }

public:
    static [[nodiscard]] std::expected<Client, ErrorMessage> create(
        const sf::IpAddress& address,
        unsigned short port,
        UpdatePlayerConfigCallback updatePlayerConfigCallback,
        UpdatePlayerInputCallback updatePlayerInputCallback)
    {
        try
        {
            auto&& client = Client(
                address,
                port,
                updatePlayerConfigCallback,
                updatePlayerInputCallback);
            if (auto&& result = client.bindToAnyPort(); !result)
                return std::unexpected(result.error());

            auto&& id = client.registerToServer();
            if (!id) return std::unexpected(id.error());
            client.myClientId = *id;

            return client;
        }
        catch (std::exception& e)
        {
            return std::unexpected(e.what());
        }
    }

    void readIncomingPackets()
    {
        sf::Packet packet;
        sf::IpAddress address;
        unsigned short port;

        while (socket->receive(packet, address, port)
               == sf::Socket::Status::Done)
        {
            auto&& result =
                handleIncomingMessage(ServerMessage::fromPacket(packet));
        }
    }

    void updatePlayerName(const std::string& newName)
    { // TODO: this
    }

    ExpectSuccess sendMapReadySignal()
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

    void sendInputUpdate(const std::string& json)
    { // TODO: this
    }

private:
    ExpectSuccess bindToAnyPort()
    {
        if (socket->bind(sf::Socket::AnyPort) != sf::Socket::Status::Done)
        {
            return std::unexpected(
                std::format("Cannot bind socket to any port"));
        }

        myPort = socket->getLocalPort();
        std::println("Socket bound to port {}", myPort);

        return ReturnFlag::Success;
    }

    std::expected<PlayerIdType, ErrorMessage> registerToServer()
    {
        if (auto&& result = sendConnectPacket(); !result)
            return std::unexpected(result.error());

        auto&& message = getConnectResponse();
        if (!message) return std::unexpected(message.error());
        return message->clientId;
    }

    ExpectSuccess sendConnectPacket()
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

    std::expected<ServerMessage, ErrorMessage> getConnectResponse()
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

    ExpectSuccess handleIncomingMessage(const ServerMessage& message)
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

private:
    sf::IpAddress remoteAddress;
    unsigned short remotePort;
    mem::Box<sf::UdpSocket> socket;
    unsigned short myPort;
    PlayerIdType myClientId;
    UpdatePlayerConfigCallback updatePlayerConfigCallback;
    UpdatePlayerInputCallback updatePlayerInputCallback;
};
