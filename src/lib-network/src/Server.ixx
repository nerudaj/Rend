module;

#pragma warning(push, 0)
#include <DGM/dgm.hpp>
#include <SFML/Network.hpp>
#pragma warning(pop)
#include <deque>
#include <format>
#include <print>
#include <stdexcept>
#include <vector>

export module Server;

import Message;
import ClientData;
import Error;

export struct ServerConfiguration final
{
    unsigned short port;
    unsigned short maxClientCount;
};

export class [[nodiscard]] Server final
{
public:
    Server(ServerConfiguration config) : MAX_CLIENT_COUNT(config.maxClientCount)
    {
        if (socket.bind(config.port) != sf::Socket::Status::Done)
        {
            throw std::runtime_error(std::format(
                "Server: Cannot bind socket to port {}", config.port));
        }

        std::println("Server: Listening on port {}", config.port);
    }

public:
    void startLoop()
    {
        socket.setBlocking(false);
        sf::IpAddress remoteAddress;
        unsigned short remotePort;
        sf::Packet packet;
        Message message;

        // Read all incoming messages
        while (true)
        {
            while (socket.receive(packet, remoteAddress, remotePort)
                   == sf::Socket::Status::Done)
            {
                handleMessage(
                    Message::parseMessage(packet), remoteAddress, remotePort);
            }

            // Send them back to all clients
            // TODO:
        }
    }

private:
    void handleMessage(
        const Message& message,
        const sf::IpAddress& address,
        unsigned short port)
    {
        switch (message.type)
        {
        case MessageType::Connect:
            if (auto&& result = handleConnectionAttempt(address, port); !result)
            {
                std::println(std::cerr, "Server: {}", result.error());
            }
            break;
        case MessageType::Update:
            std::println("Update");
            break;
        case MessageType::Disconnect:
            std::println("Disconnect");
            break;
        }
    }

    ExpectSuccess
    handleConnectionAttempt(const sf::IpAddress& address, unsigned short port)
    {
        return registeredClients.size() == MAX_CLIENT_COUNT
                   ? denyNewClient(address, port)
               : registeredClients.contains(address.toInteger())
                   ? denyReconnection(address, port)
                   : registerNewClient(address, port);
    }

    ExpectSuccess
    denyNewClient(const sf::IpAddress& address, unsigned short port)
    {
        std::println("Server: Already at full capacity, refusing new peer");

        auto&& packet =
            Message { .type = MessageType::ConnectionRefused }.toPacket();
        if (socket.send(packet, address, port) != sf::Socket::Status::Done)
        {
            return std::unexpected(std::format(
                "Failed to send ConnectionRefused to remote peer at {}:{}",
                address.toString(),
                port));
        }

        return ReturnFlag::Success;
    }

    ExpectSuccess
    denyReconnection(const sf::IpAddress& address, unsigned short port)
    {
        std::println(
            "Server: Client at {}:{} is trying to connect again, refusing",
            address.toString(),
            port);
        auto&& packet =
            Message { .type = MessageType::ConnectionRefused }.toPacket();
        if (socket.send(packet, address, port) != sf::Socket::Status::Done)
        {
            return std::unexpected(std::format(
                "Failed to send ConnectionRefused to remote peer at {}:{}",
                address.toString(),
                port));
        }

        return ReturnFlag::Success;
    }

    ExpectSuccess
    registerNewClient(const sf::IpAddress& address, unsigned short port)
    {
        auto&& newId = static_cast<PlayerIdType>(registeredClients.size());
        auto&& packet =
            Message { .type = MessageType::ConnectConfirmed, .playerId = newId }
                .toPacket();
        std::println(
            "Registering new client at {}:{} with ID: {}",
            address.toString(),
            port,
            newId);

        if (socket.send(packet, address, port) != sf::Socket::Status::Done)
        {
            return std::unexpected(std::format(
                "Could not send ConnectionConfirmed to new peer at {}:{}",
                address.toString(),
                port));
        }

        registeredClients[address.toInteger()] = newId;

        return ReturnFlag::Success;
    }

private:
    const unsigned short MAX_CLIENT_COUNT;
    sf::UdpSocket socket;
    std::deque<int> connectedClients;
    std::map<sf::Uint32, PlayerIdType> registeredClients;
};
