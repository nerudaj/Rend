#include "StdoutLogger.hpp"
#include <Client.hpp>
#include <Server.hpp>
#include <catch.hpp>
#include <thread>

constexpr const unsigned short PORT = 10666;

class MapLoaderMock final : public MapLoaderInterface
{
public:
    std::string
    loadMapInBase64(const std::string&, const std::string&) const override
    {
        return "dummy";
    }
};

void serverThread(std::atomic_bool& keepServerRunning)
{
    auto&& server = Server(
        ServerConfiguration {
            .port = PORT,
            .maxClientCount = 1,
            .acceptReconnects = true,
        },
        ServerDependencies { .logger = mem::Rc<StdoutLogger>(),
                             .mapLoader = mem::Rc<MapLoaderMock>() });

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
    auto&& result = client.readIncomingPackets(
        [&](const ServerUpdateData& update)
        {
            firstUpdateReceived = true;
            REQUIRE(update.clients.size() == 1u);
        });
    if (!result) std::cout << result.error() << std::endl;
    REQUIRE(result);
    REQUIRE(firstUpdateReceived);

    client.sendPeerSettingsUpdate(ClientData {
        .name = "custom_player",
        .hasAutoswapOnPickup = true,
    });
    sleep(100u);

    bool secondUpdateReceived = false;
    client.readIncomingPackets(
        [&](const ServerUpdateData& update)
        {
            secondUpdateReceived = true;
            REQUIRE(update.clients.size() == 1u);
            REQUIRE(update.clients.front().state == ClientState::Connected);
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
            REQUIRE(update.state == ServerState::Lobby);
        });
    REQUIRE(firstUpdateReceived);

    client.sendPeerReadySignal();
    sleep(100u);

    bool secondUpdateReceived = false;
    client.readIncomingPackets(
        [&](const ServerUpdateData& update)
        {
            secondUpdateReceived = true;
            REQUIRE(
                update.clients.front().state == ClientState::ConnectedAndReady);
            REQUIRE(update.state == ServerState::MapLoading);
        });
    REQUIRE(secondUpdateReceived);
}

TEST_CASE("reconnection uses previously cleared ClientData", "[Network]")
{
    std::atomic_bool keepServerRunning = true;
    auto&& thread = std::jthread(serverThread, std::ref(keepServerRunning));
    auto&& killswitch = Killswitch(keepServerRunning);
    sleep(100u);

    {
        auto&& client = Client("127.0.0.1", PORT);
        sleep(100u);

        bool firstUpdateReceived = false;
        client.readIncomingPackets(
            [&](const ServerUpdateData& update)
            {
                firstUpdateReceived = true;
                REQUIRE(update.state == ServerState::Lobby);
            });
        REQUIRE(firstUpdateReceived);
    }

    {
        auto&& client = Client("127.0.0.1", PORT);
        sleep(100u);

        bool firstUpdateReceived = false;
        client.readIncomingPackets(
            [&](const ServerUpdateData& update)
            {
                firstUpdateReceived = true;
                REQUIRE(update.state == ServerState::Lobby);
                REQUIRE(update.clients.size() == 1u);
            });
        REQUIRE(firstUpdateReceived);
    }
}

TEST_CASE(
    "requesting a bunch of maps returns them in individual messages",
    "[Network]")
{
    std::atomic_bool keepServerRunning = true;
    auto&& thread = std::jthread(serverThread, std::ref(keepServerRunning));
    auto&& killswitch = Killswitch(keepServerRunning);
    sleep(100u);

    {
        auto&& client = Client("127.0.0.1", PORT);
        sleep(100u);

        client.readIncomingPackets([](const ServerUpdateData&) {});
        client.requestMapDownload(
            "official_ffa", { "2p_greenpath.lvd", "2p_spacestation.lvd" });
        sleep(100u);

        int seq = 0;
        client.readIncomingPackets(
            [](const ServerUpdateData&) {},
            [&seq](const MapDownloadResponse& response)
            {
                if (seq == 0)
                {
                    REQUIRE(response.mapPackName == "official_ffa");
                    REQUIRE(response.mapName == "2p_greenpath.lvd");
                    REQUIRE_FALSE(response.base64encodedMap.empty());
                }
                else if (seq == 1)
                {
                    REQUIRE(response.mapPackName == "official_ffa");
                    REQUIRE(response.mapName == "2p_spacestation.lvd");
                    REQUIRE_FALSE(response.base64encodedMap.empty());
                }
                else
                {
                    REQUIRE(false);
                }
                ++seq;
            });
        REQUIRE(seq == 2);
    }
}
