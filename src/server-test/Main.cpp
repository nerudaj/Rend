#include <format>
#include <functional>
#include <thread>

import Network;

std::atomic_bool serverEnabled = true;

void serverLoop(Server server)
{
    while (serverEnabled)
    {
        server.update();
    }
}

int main(int, char*[])
{
    auto&& serverThread = std::thread(
        serverLoop,
        Server(ServerConfiguration { .port = 10666, .maxClientCount = 4 }));
    serverThread.join();
}