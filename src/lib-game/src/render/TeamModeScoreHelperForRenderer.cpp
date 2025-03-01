#include "render/TeamModeScoreHelperForRenderer.hpp"
#include <format>

std::string TeamModeScoreHelperForRenderer::getScoreString(
    const PlayerInventory& inventory) const
{
    const auto scores = getTeamScores();
    return std::format(
        "{} ({:+})",
        inventory.team == Team::Red ? scores.redScore : scores.blueScore,
        (scores.redScore - scores.blueScore)
            * (inventory.team == Team::Red ? 1 : -1));
}

TeamModeScoreHelperForRenderer::TeamScores
TeamModeScoreHelperForRenderer::getTeamScores() const noexcept
{
    int acc[2] = { 0, 0 };

    static_assert(std::to_underlying(Team::Red) == 1);
    static_assert(std::to_underlying(Team::Blue) == 2);

    for (const auto& state : scene.playerStates)
    {
        acc[std::to_underlying(state.inventory.team) - 1] +=
            state.inventory.score;
    }

    return TeamScores {
        .redScore = acc[0],
        .blueScore = acc[1],
    };
}
