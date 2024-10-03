#include "utils/WinConditionStrategy.hpp"

auto WinConditionStrategy::getWinConditionStrategy(const GameOptions& options)
    -> IsPointLimitReached
{
    if (options.gameMode == GameMode::SingleFlagCtf)
    {
        return [limit = options.pointlimit](const Scene& scene) -> bool
        {
            unsigned redScore = 0;
            unsigned blueScore = 0;

            for (const auto& playerState : scene.playerStates)
            {
                if (playerState.inventory.team == Team::Red)
                    redScore += playerState.inventory.score;
                else
                    blueScore += playerState.inventory.score;
            }

            return redScore >= limit || blueScore >= limit;
        };
    }

    return [limit = options.pointlimit](const Scene& scene) -> bool
    {
        return std::ranges::any_of(
            scene.playerStates,
            [limit](const PlayerState& state)
            { return state.inventory.score >= static_cast<int>(limit); });
    };
}
