#pragma once

#include <GameSettings.hpp>
#include <Memory.hpp>
#include <engine/DeathmatchSpecificRulesEngine.hpp>
#include <engine/GameModeSpecificRulesEngineInterface.hpp>
#include <engine/SingleFlagCtfSpecificRulesEngine.hpp>

class [[nodiscard]] GameModeSpecificGameRulesFactory final
{
public:
    static mem::Box<GameModeSpecificRulesEngineInterface>
    createGameModeSpecificRulesEngine(
        GameMode gameMode,
        Scene& scene,
        mem::Rc<EventQueue> eventQueue,
        const std::vector<std::string>& playerNames)
    {
        if (gameMode == GameMode::SingleFlagCtf)
            return mem::Box<SingleFlagCtfSpecificRulesEngine>(
                scene, eventQueue, playerNames);
        assert(gameMode == GameMode::Deathmatch);
        return mem::Box<DeathmatchSpecificRulesEngine>();
    }
};
