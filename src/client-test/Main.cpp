#include <SFML/Network.hpp>
#include <format>
#include <iostream>

import Network;

int main(int, char*[])
{
    auto&& client = Client::create("127.0.0.1", 10666);
    if (!client)
    {
        std::println(std::cerr, "{}", client.error());
        return 1;
    }

    return 0;
}