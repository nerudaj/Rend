#include "Server.hpp"
#include "StdoutLogger.hpp"
#include <format>
#include <functional>
#include <thread>

std::atomic_bool serverEnabled = true;

class NullMapLoader : public MapLoaderInterface
{
public:
    std::expected<std::string, Error>
    loadMapInBase64(const std::string&, const std::string&) const override
    {
        return "";
    }
};

void serverLoop(ServerConfiguration config)
{
    try
    {
        auto&& server = Server(
            config,
            ServerDependencies { .logger = mem::Rc<StdoutLogger>(),
                                 .mapLoader = mem::Rc<NullMapLoader>() });
        while (serverEnabled)
        {
            server.update();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

int main(int, char*[])
{
    auto&& serverThread = std::thread(
        serverLoop, ServerConfiguration { .port = 10666, .maxClientCount = 4 });
    serverThread.join();
}