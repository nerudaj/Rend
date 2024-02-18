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

import ServerMessage;
import ClientMessage;
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
        // Read all incoming messages
        while (true)
        {
            update();
        }
    }

    void update()
    {
        socket.setBlocking(false);
        sf::IpAddress remoteAddress;
        unsigned short remotePort;
        sf::Packet packet;

        while (socket.receive(packet, remoteAddress, remotePort)
               == sf::Socket::Status::Done)
        {
            handleMessage(
                ClientMessage::fromPacket(packet), remoteAddress, remotePort);
        }

        // Send them back to all clients
        // TODO:
    }

private:
    void handleMessage(
        const ClientMessage& message,
        const sf::IpAddress& address,
        unsigned short port)
    {
        switch (message.type)
        {
            using enum ClientMessageType;
        case ConnectionRequest:
            if (auto&& result = handleConnectionAttempt(address, port); !result)
            {
                std::println(std::cerr, "Server: {}", result.error());
            }
            break;
        case PeerSettingsUpdate:
        case GameSettingsUpdate:
        case CommitLobby:
        case MapLoaded:
        case ReportInput:
        case Disconnect:
            std::println("Other message");
            break;
        default:
            std::println(
                std::cerr,
                "Unknown message {}",
                static_cast<std::underlying_type_t<ClientMessageType>>(
                    message.type));
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
            ServerMessage { .type = ServerMessageType::ConnectionRefused }
                .toPacket();
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
            ServerMessage { .type = ServerMessageType::ConnectionRefused }
                .toPacket();
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
            ServerMessage { .type = ServerMessageType::ConnectionAccepted,
                            .clientId = newId }
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
        if (newId == 0)
        {
            adminAddress = address.toInteger();
        }

        return ReturnFlag::Success;
    }

private:
    const unsigned short MAX_CLIENT_COUNT;
    sf::UdpSocket socket;
    std::map<sf::Uint32, PlayerIdType> registeredClients;
    sf::Uint32 adminAddress; // first connected client is the admin
};
