module;

#include <nlohmann/json.hpp>

export module InputData;

export import InputSchema;
export import NetworkTypes;

export
{
    struct [[nodiscard]] InputData
    {
        PlayerIdType clientId;
        size_t tick;
        InputSchema input;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InputData, clientId, tick, input);
}