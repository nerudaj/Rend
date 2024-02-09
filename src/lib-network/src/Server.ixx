module;

#pragma warning(push, 0)
#include <DGM/dgm.hpp>
#include <SFML/Network.hpp>
#pragma warning(pop)
#include <deque>
#include <format>
#include <stdexcept>
#include <vector>

export module Server;

import Message;
import ClientData;

export class [[nodiscard]] Server final
{
public:
    Server(unsigned short port)
    {
        if (socket.bind(port) != sf::Socket::Status::Done)
        {
            throw std::runtime_error(
                std::format("Server: Cannot bind socket to port {}", port));
        }
    }

private:
    void startLoop()
    {
        socket.setBlocking(false);
        sf::IpAddress sender;
        unsigned short port;
        sf::Packet packet;
        Message message;

        // Read all incoming messages
        while (socket.receive(packet, sender, port) == sf::Socket::Status::Done)
        {
            packet >> message;
            handleMessage(message);
        }

        // Send them back to all clients
        // TODO:
    }

    void handleMessage(const Message& message)
    {
        switch (message.type)
        {
        case MessageType::Connect:
            break;
        case MessageType::Update:
            break;
        case MessageType::Disconnect:
            break;
        }
    }

private:
    sf::UdpSocket socket;
    std::deque<int> connectedClients;
};
