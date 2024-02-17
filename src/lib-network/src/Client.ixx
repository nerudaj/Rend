module;

#pragma warning(push, 0)
#include <SFML/Network.hpp>
#pragma warning(pop)
#include <cassert>
#include <concepts>
#include <expected>
#include <format>
#include <print>
#include <stdexcept>
#include <string>

export module Client;

import Message;
import Error;
import Memory;

export class Client
{
public:
    Client(const sf::IpAddress& address, unsigned short port)
        : remoteAddress(address), remotePort(port)
    {
    }

public:
    static [[nodiscard]] std::expected<Client, ErrorMessage>
    create(const sf::IpAddress& address, unsigned short port)
    {
        try
        {
            auto&& client = Client(address, port);
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
        return message->playerId;
    }

    ExpectSuccess sendConnectPacket()
    {
        auto&& packet = Message { .type = MessageType::Connect }.toPacket();
        if (socket->send(packet, remoteAddress, remotePort)
            != sf::Socket::Status::Done)
        {
            return std::unexpected(std::format(
                "Could not send message to {}:{}",
                remoteAddress.toString(),
                remotePort));
        }
    }

    std::expected<Message, ErrorMessage> getConnectResponse()
    {
        auto&& packet = sf::Packet();
        if (socket->receive(packet, remoteAddress, remotePort)
            != sf::Socket::Status::Done)
        {
            return std::unexpected(
                std::format("Got no response from remote server"));
        }

        auto&& message = Message::parseMessage(packet);
        if (message.type != MessageType::ConnectConfirmed)
        {
            return std::unexpected(std::format(
                "Expected ConnectConfirmed from the server, got {}",
                static_cast<std::underlying_type_t<MessageType>>(
                    message.type)));
        }

        return message;
    }

private:
    sf::IpAddress remoteAddress;
    unsigned short remotePort;
    mem::Box<sf::UdpSocket> socket;
    unsigned short myPort;
    PlayerIdType myClientId;
};
