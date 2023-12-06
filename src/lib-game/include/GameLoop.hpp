#pragma once

#include <DGM/dgm.hpp>
#include <core/Scene.hpp>
#include <engine/AiEngine.hpp>
#include <engine/AnimationEngine.hpp>
#include <engine/AudioEngine.hpp>
#include <engine/GameRulesEngine.hpp>
#include <engine/PhysicsEngine.hpp>
#include <engine/RenderingEngine.hpp>
#include <events/EventQueue.hpp>

import Memory;

class [[nodiscard]] GameLoop final
{
public:
    GameLoop(
        Scene& scene,
        mem::Rc<EventQueue> eventQueue,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<AudioPlayer> audioPlayer,
        const RenderSettings& renderSettings)
        : scene(scene)
        , eventQueue(eventQueue)
        , aiEngine(scene)
        , animationEngine(scene, eventQueue)
        , audioEngine(resmgr, audioPlayer, scene)
        , gameRulesEngine(scene, eventQueue)
        , physicsEngine(scene, eventQueue)
        , renderingEngine(renderSettings, *resmgr, scene)
    {
    }

public:
    void update(const float dt, bool skipAudio);

    void renderTo(dgm::Window& window);

    const GameRulesEngine& getRulesEngine() const
    {
        return gameRulesEngine;
    }

    [[nodiscard]] bool isPointlimitReached(unsigned limit) const;

private:
    void updateEngines(const float dt);

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
