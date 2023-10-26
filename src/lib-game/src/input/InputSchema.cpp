#include <input/InputSchema.hpp>

void to_json(nlohmann::json& j, const InputSchema& schema)
{
    j = nlohmann::json {
        { "f", schema.shooting },    { "pw", schema.prevWeapon },
        { "nw", schema.lastWeapon }, { "lw", schema.prevWeapon },
        { "ft", schema.thrust },     { "st", schema.sidewardThrust },
        { "lr", schema.steer }
    };
}

void from_json(const nlohmann::json& json, InputSchema& schema)
{
    json.at("f").get_to(schema.shooting);
    json.at("pw").get_to(schema.prevWeapon);
    json.at("nw").get_to(schema.nextWeapon);
    json.at("lw").get_to(schema.lastWeapon);
    json.at("ft").get_to(schema.thrust);
    json.at("st").get_to(schema.sidewardThrust);
    json.at("lr").get_to(schema.steer);
}
