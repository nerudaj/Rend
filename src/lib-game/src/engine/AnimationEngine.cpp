#include <core/Constants.hpp>
#include <core/EntityDefinitions.hpp>
#include <engine/AnimationEngine.hpp>
#include <events/EventQueue.hpp>
#include <events/GameRuleEvents.hpp>

void AnimationEngine::operator()(const SetStateAnimationEvent&) {}

void AnimationEngine::update(const float)
{
    for (auto&& [thing, idx] : scene.things)
    {
        if (!ENTITY_PROPERTIES.contains(thing.typeId)) continue;

        const auto& eprop = ENTITY_PROPERTIES.at(thing.typeId);
        if (eprop.states.empty()) continue;

        auto& state = eprop.states.at(thing.animationStateId);

        bool shouldUpdate = (scene.frameId - thing.lastAnimationUpdate)
                            == state.updateFrequency;
        if (!shouldUpdate) continue;

        thing.lastAnimationUpdate = scene.frameId;
        ++thing.animationFrameIndex;

        if (thing.animationFrameIndex == state.clip.size())
            handleTransition(thing, idx, state);
        else
            updateSpriteId(thing, state);
    }
}

void AnimationEngine::handleTransition(
    Entity& entity, std::size_t entityIdx, const AnimationState& oldState)
{
    switch (oldState.transition)
    {
        using enum AnimationStateId;

    case MarkerLoop:
        entity.animationFrameIndex = 0;
        updateSpriteId(entity, oldState);
        break;
    case MarkerDestroy:
        EventQueue::add<EntityDestroyedGameEvent>(entityIdx);
        break;
    case MarkerFreeze:
        entity.typeId =
            EntityType::EffectStatic; // disable further animation handling
        break;
    default: {
        // Switch to new state
        auto& newState =
            ENTITY_PROPERTIES.at(entity.typeId).states.at(oldState.transition);
        entity.animationStateId = oldState.transition;
        entity.animationFrameIndex = 0;
        updateSpriteId(entity, newState);
    }
    }
}

void AnimationEngine::updateSpriteId(
    Entity& entity, const AnimationState& state)
{
    assert(state.clip.size() > entity.animationFrameIndex);
    entity.spriteClipIndex = state.clip[entity.animationFrameIndex];
}
