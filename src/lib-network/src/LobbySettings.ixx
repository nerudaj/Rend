module;

#include <nlohmann/json.hpp>
#include <string>

export module LobbySettings;

export
{
    struct [[nodiscard]] LobbySettings
    {
        std::string mapname;
        int fraglimit;
        unsigned playerCount;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        LobbySettings, mapname, fraglimit, playerCount);
}