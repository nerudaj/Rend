#pragma once

#include "render/ScoreHelperForRendererInterface.hpp"
#include <format>

class [[nodiscard]] TeamModeScoreHelperForRenderer final
    : public ScoreHelperForRendererInterface
{
public:
    constexpr explicit TeamModeScoreHelperForRenderer(
        const Scene& scene) noexcept
        : scene(scene)
    {
    }

public:
    [[nodiscard]] std::string
    getScoreString(const PlayerInventory& inventory) const
    {
        const auto myTeamScore = getScoreForTeam(inventory.team);
        return std::format(
            "{} ({:+})",
            myTeamScore,
            myTeamScore
                - getScoreForTeam(
                    inventory.team == Team::Red ? Team::Blue : Team::Red));
    }

private:
    constexpr [[nodiscard]] int getScoreForTeam(Team team) const noexcept
    {
        int acc = 0;
        for (size_t i = (team == Team::Red ? 0 : 1);
             i < scene.playerStates.size();
             i += 2) // iterate only over members of the same team
        {
            acc += scene.playerStates[i].inventory.score;
        }
        return acc;
    }

private:
    const Scene& scene;
};