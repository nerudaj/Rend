#include "Server.hpp"
#include <format>
#include <functional>
#include <thread>

std::atomic_bool serverEnabled = true;

void serverLoop(ServerConfiguration config)
{
    try
    {
        auto&& server = Server(config);
        while (serverEnabled)
        {
            server.update([](const std::string& log)
                          { std::cout << log << std::endl; });
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