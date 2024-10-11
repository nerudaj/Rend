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
    struct [[nodiscard]] TeamScores final
    {
        int redScore = 0;
        int blueScore = 0;
    };

public:
    [[nodiscard]] std::string
    getScoreString(const PlayerInventory& inventory) const;

    [[nodiscard]] TeamScores getTeamScores() const noexcept;

private:
    const Scene& scene;
};