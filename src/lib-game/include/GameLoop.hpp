#pragma once

#include "AudioPlayer.hpp"
#include <DGM/dgm.hpp>
#include <Memory.hpp>
#include <core/Scene.hpp>
#include <engine/AiEngine.hpp>
#include <engine/AnimationEngine.hpp>
#include <engine/AudioEngine.hpp>
#include <engine/GameRulesEngine.hpp>
#include <engine/PhysicsEngine.hpp>
#include <engine/RenderingEngine.hpp>
#include <events/EventQueue.hpp>
#include <ranges>
#include <utils/GameModeSpecificGameRulesFactory.hpp>

class [[nodiscard]] GameLoop final
{
public:
    GameLoop(
        Scene& scene,
        mem::Rc<EventQueue> eventQueue,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<AudioPlayer> audioPlayer,
        const std::vector<std::string>& playerNames,
        GameMode gameMode,
        const DisplayOptions& renderSettings,
        bool useNullBotBehavior)
        : scene(scene)
        , eventQueue(eventQueue)
        , modeSpecificRules(GameModeSpecificGameRulesFactory::
                                createGameModeSpecificRulesEngine(
                                    gameMode, scene, eventQueue, playerNames))
        , aiEngine(scene, useNullBotBehavior)
        , animationEngine(scene, eventQueue)
        , audioEngine(audioPlayer, scene)
        , gameRulesEngine(scene, eventQueue, *modeSpecificRules, playerNames)
        , physicsEngine(scene, eventQueue)
        , renderingEngine(renderSettings, *resmgr, scene)
    {
    }

public:
    void update(const float dt, const float realDt, bool skipAudio);

    void renderTo(dgm::Window& window);

    /*const GameRulesEngine& getRulesEngine() const
    {
        return gameRulesEngine;
    }*/

private:
    void updateEngines(const float dt, const float realDt);

    void processEvents(const bool skipAudio);

private:
    Scene& scene;
    mem::Rc<EventQueue> eventQueue;
    mem::Box<GameModeSpecificRulesEngineInterface> modeSpecificRules;
    AiEngine aiEngine;
    AnimationEngine animationEngine;
    AudioEngine audioEngine;
    GameRulesEngine gameRulesEngine;
    PhysicsEngine physicsEngine;
    RenderingEngine renderingEngine;
};
