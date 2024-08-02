#pragma once

#include <SFML\Config.hpp>
#include <string>
#include <variant>

struct [[nodiscard]] PeerLeftServerEvent final
{
    sf::Uint64 clientKey;
};

struct [[nodiscard]] MapRequestedServerEvent final
{
    sf::Uint64 clientKey;
    std::string mapPackName;
    std::string mapName;
};

using ServerEvents = std::variant<PeerLeftServerEvent, MapRequestedServerEvent>;
