#include "GameLoop.hpp"

void GameLoop::update(const float dt, bool skipAudio)
{
    updateEngines(dt);
    processEvents(skipAudio);
    gameRulesEngine.deleteMarkedObjects();
    aiEngine.update(dt); // must happen after everything else
}

void GameLoop::renderTo(dgm::Window& window)
{
    renderingEngine.renderTo(window);
}

bool GameLoop::isPointlimitReached(unsigned limit) const
{
    for (auto&& state : scene.playerStates)
    {
        if (state.inventory.score >= static_cast<int>(limit))
        {
            return true;
        }
    }
    return false;
}

void GameLoop::updateEngines(const float dt)
{
    animationEngine.update(dt);
    audioEngine.update(dt);
    physicsEngine.update(dt);
    gameRulesEngine.update(dt);
    renderingEngine.update(dt);
}

void GameLoop::processEvents(const bool skipAudio)
{
    // Animation, Physics and Game can produce GameEvent
    eventQueue->processAndClear(animationEngine);
    eventQueue->processAndClear(gameRulesEngine);
    assert(eventQueue->isEmpty<AnimationEvent>());

    // Audio can only produce Audio events
    if (skipAudio)
    {
        eventQueue->processAndClear(audioEngine);
    }
    else
    {
        eventQueue->clear<AudioEvent>();
    }
}
