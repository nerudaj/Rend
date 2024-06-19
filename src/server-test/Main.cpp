#include "Server.hpp"
#include <format>
#include <functional>
#include <thread>

std::atomic_bool serverEnabled = true;

void serverLoop(Server server)
{
    while (serverEnabled)
    {
        server.update([](const std::string& log)
                      { std::cout << log << std::endl; });
    }
}

int main(int, char*[])
{
    auto&& serverThread = std::thread(
        serverLoop,
        Server(ServerConfiguration { .port = 10666, .maxClientCount = 4 }));
    serverThread.join();
}