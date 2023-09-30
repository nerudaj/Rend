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
        // Can be removed if there is a definition for EffectStatic with
        // NoRender sprite
        if (!ENTITY_PROPERTIES.contains(thing.typeId)) continue;

        const auto& eprop = ENTITY_PROPERTIES.at(thing.typeId);
        if (eprop.states.empty()) continue;

        auto& state = eprop.states.at(thing.renderState.animationStateId);

        bool shouldUpdate =
            (scene.frameId - thing.renderState.lastAnimationUpdate)
            == state.updateFrequency;
        if (!shouldUpdate) continue;

        thing.renderState.lastAnimationUpdate = scene.frameId;
        ++thing.renderState.animationFrameIndex;

        if (thing.renderState.animationFrameIndex == state.clip.size())
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
        entity.renderState.animationFrameIndex = 0;
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
        entity.renderState.animationStateId = oldState.transition;
        entity.renderState.animationFrameIndex = 0;
        updateSpriteId(entity, newState);
    }
    }
}

void AnimationEngine::updateSpriteId(
    Entity& entity, const AnimationState& state)
{
    assert(state.clip.size() > entity.renderState.animationFrameIndex);
    entity.renderState.spriteClipIndex =
        state.clip[entity.renderState.animationFrameIndex];
}
