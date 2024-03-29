module;

#pragma warning(push, 0)
#include <DGM/dgm.hpp>
#include <SFML/Network.hpp>
#pragma warning(pop)
#include <expected>
#include <map>
#include <string>

export module Server;

export import ClientMessage;
export import Error;
export import Memory;
export import ServerMessage;

export struct ServerConfiguration final
{
    unsigned short port;
    unsigned short maxClientCount;
    bool acceptReconnects = false;
};

export class [[nodiscard]] Server final
{
public:
    Server(ServerConfiguration config);

public:
    void update();

private:
    ExpectedLog handleMessage(
        const ClientMessage& message,
        const sf::IpAddress& address,
        unsigned short port);

    ExpectedLog
    handleNewConnection(const sf::IpAddress& address, unsigned short port);

    ExpectedLog
    handleLobbyCommited(const sf::IpAddress& address, unsigned short port);

    ExpectedLog
    handleMapReady(const sf::IpAddress& address, unsigned short port);

    ExpectedLog
    handleMapEnded(const sf::IpAddress& address, unsigned short port);

    ExpectedLog handleReportedInput(const ClientMessage& message);

    ExpectedLog handleLobbySettingsUpdate(const ClientMessage& message);

    ExpectedLog
    handleDisconnection(const sf::IpAddress& address, unsigned short port);

    ExpectSuccess
    denyNewPeer(const sf::IpAddress& address, unsigned short port);

    [[nodiscard]] bool isAdmin(const sf::IpAddress& address) const noexcept
    {
        return isRegistered(address)
               && registeredClients.at(address.toInteger()).id == 0u;
    }

    [[nodiscard]] bool isRegistered(const sf::IpAddress& address) const noexcept
    {
        return registeredClients.contains(address.toInteger());
    }

    void updateMapEndedStatuses();

    [[nodiscard]] constexpr bool areAllPeersReady() const noexcept
    {
        return std::all_of(
            registeredClients.begin(),
            registeredClients.end(),
            [](auto&& client) { return client.second.ready; });
    }

    [[nodiscard]] constexpr bool isNoPeerReady() const noexcept
    {
        return std::none_of(
            registeredClients.begin(),
            registeredClients.end(),
            [](auto&& client) { return client.second.ready; });
    }

private:
    struct ClientConnectionInfo
    {
        PlayerIdType id;
        sf::IpAddress address;
        unsigned short port;
        bool ready = false;
    };

    const unsigned short MAX_CLIENT_COUNT;
    const bool ACCEPT_RECONNECTS;
    mem::Box<sf::UdpSocket> socket;
    std::map<sf::Uint32, ClientConnectionInfo> registeredClients;
    ServerUpdateData updateData;
    size_t sequence = 0;
    size_t mapReadyCounter = 0;
};
