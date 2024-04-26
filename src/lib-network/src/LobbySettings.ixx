module;

#include <nlohmann/json.hpp>
#include <string>

export module LobbySettings;

export
{
    struct [[nodiscard]] MapSettings
    {
        std::string name;
        bool enabled;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapSettings, name, enabled);

    struct [[nodiscard]] LobbySettings
    {
        std::string packname;
        std::vector<MapSettings> mapSettings;
        std::vector<size_t> mapOrder;
        int fraglimit;
        unsigned playerCount;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        LobbySettings, packname, mapSettings, mapOrder, fraglimit, playerCount);
}