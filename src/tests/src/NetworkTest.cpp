#include <Client.hpp>
#include <Server.hpp>
#include <catch.hpp>
#include <thread>

constexpr const unsigned short PORT = 10666;

void serverThread(std::atomic_bool& keepServerRunning)
{
    auto&& server = Server(ServerConfiguration {
        .port = PORT,
        .maxClientCount = 1,
        .acceptReconnects = true,
    });

    while (keepServerRunning)
    {
        server.update();
    }
}

void sleep(unsigned milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

class Killswitch
{
public:
    Killswitch(std::atomic_bool& keepServerRunning)
        : keepServerRunning(keepServerRunning)
    {
    }

    ~Killswitch()
    {
        keepServerRunning = false;
        sleep(100u);
    }

private:
    std::atomic_bool& keepServerRunning;
};

TEST_CASE("Client info is correctly propagated", "[Network]")
{
    std::atomic_bool keepServerRunning = true;
    auto&& thread = std::jthread(serverThread, std::ref(keepServerRunning));
    auto&& killswitch = Killswitch(keepServerRunning);
    sleep(100u);

    auto&& client = Client("127.0.0.1", PORT);
    sleep(100u);

    bool firstUpdateReceived = false;
    client.readIncomingPackets(
        [&](const ServerUpdateData& update)
        {
            firstUpdateReceived = true;
            REQUIRE(update.clients.size() == 1u);
        });
    REQUIRE(firstUpdateReceived);

    client.sendPeerUpdate(ClientData {
        .name = "custom_player",
        .isReady = true,
        .hasAutoswapOnPickup = true,
    });
    sleep(100u);

    bool secondUpdateReceived = false;
    client.readIncomingPackets(
        [&](const ServerUpdateData& update)
        {
            secondUpdateReceived = true;
            REQUIRE(update.clients.size() == 1u);
            REQUIRE(update.clients.front().active);
            REQUIRE(update.clients.front().isReady);
            REQUIRE(update.clients.front().name == "custom_player");
            REQUIRE(update.clients.front().hasAutoswapOnPickup);
        });
    REQUIRE(secondUpdateReceived);
}

TEST_CASE("peersReady is correctly computed", "[Network]")
{

    std::atomic_bool keepServerRunning = true;
    auto&& thread = std::jthread(serverThread, std::ref(keepServerRunning));
    auto&& killswitch = Killswitch(keepServerRunning);
    sleep(100u);

    auto&& client = Client("127.0.0.1", PORT);
    sleep(100u);

    bool firstUpdateReceived = false;
    client.readIncomingPackets(
        [&](const ServerUpdateData& update)
        {
            firstUpdateReceived = true;
            REQUIRE_FALSE(update.peersReady);
        });
    REQUIRE(firstUpdateReceived);

    client.sendPeerUpdate(ClientData {
        .isReady = true,
    });
    sleep(100u);

    bool secondUpdateReceived = false;
    client.readIncomingPackets(
        [&](const ServerUpdateData& update)
        {
            secondUpdateReceived = true;
            REQUIRE(update.peersReady);
        });
    REQUIRE(secondUpdateReceived);
}
