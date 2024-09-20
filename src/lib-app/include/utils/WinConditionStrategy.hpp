#pragma once

#include <GameSettings.hpp>
#include <core/Scene.hpp>
#include <functional>

using IsPointLimitReached = std::function<bool(const Scene& scene)>;

class [[nodiscard]] WinConditionStrategy final
{
public:
    static [[nodiscard]] auto
    getWinConditionStrategy(const GameOptions& options) -> IsPointLimitReached;
};
