module;

#include <nlohmann/json.hpp>
#include <string>

export module MapData;

export
{
    struct [[nodiscard]] MapData
    {
        std::string name;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData, name);
}