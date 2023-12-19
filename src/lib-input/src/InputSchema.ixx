module;

#include <nlohmann/json.hpp>

export module InputSchema;

export
{
    struct InputSchema
    {
        bool shooting = false;
        bool prevWeapon = false;
        bool nextWeapon = false;
        bool lastWeapon = false;
        float thrust = 0.f;
        float sidewardThrust = 0.f;
        float steer = 0.f;
    };

    void to_json(nlohmann::json & j, const InputSchema&);
    void from_json(const nlohmann::json&, InputSchema&);
}
