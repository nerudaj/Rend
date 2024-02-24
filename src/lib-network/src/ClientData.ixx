module;

#include <nlohmann/json.hpp>
#include <string>

export module ClientData;

export
{
    struct [[nodiscard]] ClientData
    {
        bool active = true;
        std::string name;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ClientData, active, name);
}