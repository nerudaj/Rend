#pragma once

#include "render/ScoreHelperForRendererInterface.hpp"
#include <format>

class [[nodiscard]] DeathmatchScoreHelperForRenderer final
    : public ScoreHelperForRendererInterface
{
public:
    constexpr explicit DeathmatchScoreHelperForRenderer(
        const Scene& scene) noexcept
        : scene(scene)
    {
    }

public:
    [[nodiscard]] std::string
    getScoreString(const PlayerInventory& inventory) const
    {
        return std::format(
            "{} ({:+})", inventory.score, getScoreOffset(inventory.score));
    }

private:
    constexpr [[nodiscard]] int getScoreOffset(int score) const noexcept
    {
        int bestScore = std::numeric_limits<int>::min();
        int secondBestScore = std::numeric_limits<int>::min();

        for (const auto& state : scene.playerStates)
        {
            if (state.inventory.score > bestScore)
            {
                secondBestScore = bestScore;
                bestScore = state.inventory.score;
            }
            else if (state.inventory.score > secondBestScore)
            {
                secondBestScore = state.inventory.score;
            }
        }

        return score == bestScore ? score - secondBestScore : score - bestScore;
    }

private:
    const Scene& scene;
};
