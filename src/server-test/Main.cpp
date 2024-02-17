#include <format>

import Network;

int main(int, char*[])
{
    auto&& server =
        Server(ServerConfiguration { .port = 10666, .maxClientCount = 4 });
    server.startLoop();
}