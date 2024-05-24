#include "GameLoop.hpp"

void GameLoop::update(const float dt, const float realDt, bool skipAudio)
{
    updateEngines(dt, realDt);
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
    return std::ranges::any_of(
        scene.playerStates,
        [limit](const PlayerState& state)
        { return state.inventory.score >= static_cast<int>(limit); });
}

void GameLoop::updateEngines(const float dt, const float realDt)
{
    animationEngine.update(dt);
    audioEngine.update(dt);
    physicsEngine.update(dt);
    gameRulesEngine.update(dt);
    renderingEngine.update(dt, realDt);
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
