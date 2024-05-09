#include "Client.hpp"
#include <Memory.hpp>
#include <SFML/Network.hpp>
#include <format>
#include <iostream>

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