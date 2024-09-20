#include <core/Constants.hpp>
#include <core/EntityDefinitions.hpp>
#include <engine/AnimationEngine.hpp>
#include <events/EventQueue.hpp>
#include <events/GameRuleEvents.hpp>

void AnimationEngine::operator()(const PlayerIsRunningAnimationEvent& e)
{
    if (scene.things[e.playerIdx].animationContext.animationStateId
        == AnimationStateId::Idle)
        setEntityAnimationState(e.playerIdx, AnimationStateId::Run);
}

void AnimationEngine::operator()(const PlayerFiredAnimationEvent& e)
{
    setEntityAnimationState(e.playerIdx, AnimationStateId::Missile);
    setWeaponAnimationState(e.playerIdx, AnimationStateId::Missile);
}

void AnimationEngine::operator()(const WeaponSwappedAnimationEvent& e)
{
    setWeaponAnimationState(e.playerIdx, e.animationId);
}

void AnimationEngine::update(const float)
{
    for (auto&& [thing, idx] : scene.things)
    {
        handleUpdate(thing.animationContext, thing.typeId, idx);

        if (thing.typeId == EntityType::Player)
        {
            auto& inventory = scene.playerStates[thing.stateIdx].inventory;
            handleUpdate(
                inventory.animationContext,
                inventory.activeWeaponType,
                inventory.ownerIdx);
        }
    }
}

void AnimationEngine::handleUpdate(
    AnimationContext& context, EntityType entityType, std::size_t idx)
{
    const auto& eprop = ENTITY_PROPERTIES.at(entityType);
    if (eprop.states.empty()) return;

    auto& state = eprop.states.at(context.animationStateId);

    bool shouldUpdate = (scene.tick - context.lastAnimationUpdate)
                        >= state.clip[context.animationFrameIndex].duration;
    if (!shouldUpdate) return;

    if ((context.animationFrameIndex + 1) == state.clip.size())
        handleTransition(context, entityType, idx, state);
    else
    {
        ++context.animationFrameIndex;
        updateSpriteId(context, state, idx);
    }
}

void AnimationEngine::handleTransition(
    AnimationContext& context,
    EntityType entityType,
    EntityIndexType entityIdx,
    const AnimationState& oldState)
{
    switch (oldState.transition)
    {
        using enum AnimationStateId;

    case Raise:
        eventQueue->emplace<WeaponSwappedAnimationEvent>(entityIdx, Raise);
        break;
    case FastRaise:
        eventQueue->emplace<WeaponSwappedAnimationEvent>(entityIdx, FastRaise);
        break;
    case MarkerLoop:
        context.animationFrameIndex = 0;
        updateSpriteId(context, oldState, entityIdx);
        break;
    case MarkerDestroy:
        eventQueue->emplace<EntityDestroyedGameEvent>(entityIdx);
        break;
    default: {
        // Switch to new state
        auto& newState =
            ENTITY_PROPERTIES.at(entityType).states.at(oldState.transition);
        context.animationStateId = oldState.transition;
        context.animationFrameIndex = 0;
        updateSpriteId(context, newState, entityIdx);
    }
    }
}

void AnimationEngine::setWeaponAnimationState(
    EntityIndexType playerIdx, AnimationStateId state)
{
    auto&& inventory = getInventory(playerIdx);

    using enum AnimationStateId;
    if (state == Raise || state == FastRaise)
    {
        // This should trigger some new game rule event, but to hell with it
        assert(inventory.activeWeaponType != inventory.lastWeaponType);
        std::swap(inventory.activeWeaponType, inventory.lastWeaponType);
    }

    inventory.animationContext.animationStateId = state;
    inventory.animationContext.animationFrameIndex = 0;
    updateSpriteId(
        inventory.animationContext,
        ENTITY_PROPERTIES.at(inventory.activeWeaponType)
            .states.at(inventory.animationContext.animationStateId),
        playerIdx);
}

void AnimationEngine::setEntityAnimationState(
    EntityIndexType idx, AnimationStateId state)
{
    auto& context = scene.things[idx].animationContext;
    if (context.animationStateId == state) return;
    context.animationStateId = state;
    context.animationFrameIndex = 0;
    updateSpriteId(
        context,
        ENTITY_PROPERTIES.at(scene.things[idx].typeId).states.at(state),
        idx);
}

void AnimationEngine::updateSpriteId(
    AnimationContext& context, const AnimationState& state, EntityIndexType idx)
{
    assert(state.clip.size() > context.animationFrameIndex);
    context.lastAnimationUpdate = scene.tick;
    context.spriteClipIndex = state.clip[context.animationFrameIndex].spriteId;

    auto& script = state.clip[context.animationFrameIndex].scriptToTrigger;
    if (script.id != ScriptId::NoAction)
        eventQueue->emplace<ScriptTriggeredGameEvent>(script, idx);
}
