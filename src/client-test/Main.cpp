#include "Client.hpp"
#include <SFML/Network.hpp>
#include <format>
#include <iostream>

import Memory;

struct A
{
    A() : client(mem::Rc<Client>("127.0.0.1", 10666ui16)) {}

    mem::Rc<Client> client;
};

int main(int, char*[])
{
    A a;
    return 0;
}