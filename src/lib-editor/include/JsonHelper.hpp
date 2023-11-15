#pragma once

#include <DGM/dgm.hpp>
#include <nlohmann/json.hpp>

class JsonHelper
{
public:
    static nlohmann::json loadFromFile(const std::string& path);

    static sf::Vector2u arrayToVector2u(nlohmann::json& arr)
    {
        return sf::Vector2u(arr[0], arr[1]);
    }

    static sf::IntRect arrayToIntRect(nlohmann::json& arr)
    {
        return sf::IntRect(arr[0], arr[1], arr[2], arr[3]);
    }
};