#pragma once

#include "AudioPlayer.hpp"
#include <CmdParameters.hpp>
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

class [[nodiscard]] GameLoop final
{
public:
    GameLoop(
        Scene& scene,
        mem::Rc<EventQueue> eventQueue,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<AudioPlayer> audioPlayer,
        const std::vector<std::string>& playerNames,
        const DisplayOptions& renderSettings,
        const CmdParameters& cmdParams)
        : scene(scene)
        , eventQueue(eventQueue)
        , aiEngine(
              scene,
              AiEngineConfig {
                  .useNullBehavior = cmdParams.useNullBotBehavior,
                  .enableLogging = cmdParams.enableDebug,
              })
        , animationEngine(scene, eventQueue)
        , audioEngine(audioPlayer, scene)
        , gameRulesEngine(scene, eventQueue, playerNames)
        , physicsEngine(scene, eventQueue)
        , renderingEngine(renderSettings, *resmgr, scene)
    {
    }

public:
    void update(const float dt, const float realDt, bool skipAudio);

    void renderTo(dgm::Window& window);

    const GameRulesEngine& getRulesEngine() const
    {
        return gameRulesEngine;
    }

    [[nodiscard]] bool isPointlimitReached(unsigned limit) const;

private:
    void updateEngines(const float dt, const float realDt);

    void processEvents(const bool skipAudio);

private:
    Scene& scene;
    mem::Rc<EventQueue> eventQueue;
    AiEngine aiEngine;
    AnimationEngine animationEngine;
    AudioEngine audioEngine;
    GameRulesEngine gameRulesEngine;
    PhysicsEngine physicsEngine;
    RenderingEngine renderingEngine;
};
