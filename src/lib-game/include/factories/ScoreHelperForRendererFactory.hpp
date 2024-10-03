#pragma once

#include "render/DeathmatchScoreHelperForRenderer.hpp"
#include "render/ScoreHelperForRendererInterface.hpp"
#include "render/TeamModeScoreHelperForRenderer.hpp"
#include <Memory.hpp>
#include <enums/GameMode.hpp>

class [[nodiscard]] ScoreHelperForRendererFactory final
{
public:
    static [[nodiscard]] mem::Box<ScoreHelperForRendererInterface>
    createScoreHelper(GameMode gameMode, const Scene& scene)
    {
        if (gameMode == GameMode::SingleFlagCtf)
        {
            return mem::Box<TeamModeScoreHelperForRenderer>(scene);
        }

        assert(gameMode == GameMode::Deathmatch);
        return mem::Box<DeathmatchScoreHelperForRenderer>(scene);
    }
};
