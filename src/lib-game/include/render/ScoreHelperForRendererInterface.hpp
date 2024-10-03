#pragma once

#include "core/Scene.hpp"

class [[nodiscard]] ScoreHelperForRendererInterface
{
public:
    virtual ~ScoreHelperForRendererInterface() = default;

public:
    virtual [[nodiscard]] std::string
    getScoreString(const PlayerInventory& inventory) const = 0;
};
