module;

#pragma warning(push, 0)
#include <DGM/dgm.hpp>
#include <SFML/Network.hpp>
#pragma warning(pop)
#include <map>

export module Server;

export import ClientMessage;
export import Error;
export import Memory;

export struct ServerConfiguration final
{
    unsigned short port;
    unsigned short maxClientCount;
};

export class [[nodiscard]] Server final
{
public:
    Server(ServerConfiguration config);

public:
    void update();

private:
    void handleMessage(
        const ClientMessage& message,
        const sf::IpAddress& address,
        unsigned short port);

    ExpectSuccess
    handleConnectionAttempt(const sf::IpAddress& address, unsigned short port);

    ExpectSuccess
    denyNewClient(const sf::IpAddress& address, unsigned short port);

    ExpectSuccess
    denyReconnection(const sf::IpAddress& address, unsigned short port);

    ExpectSuccess
    registerNewClient(const sf::IpAddress& address, unsigned short port);

private:
    const unsigned short MAX_CLIENT_COUNT;
    mem::Box<sf::UdpSocket> socket;
    std::map<sf::Uint32, PlayerIdType> registeredClients;
    sf::Uint32 adminAddress; // first connected client is the admin
};
