#pragma once

#include <nlohmann/json.hpp>
#include <string>

struct NetOptions
{
    std::string joinIpAddress = "192.168.0.1";
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NetOptions, joinIpAddress);
