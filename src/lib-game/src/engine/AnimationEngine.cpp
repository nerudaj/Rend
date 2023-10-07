#include <core/Constants.hpp>
#include <core/EntityDefinitions.hpp>
#include <engine/AnimationEngine.hpp>
#include <events/EventQueue.hpp>
#include <events/GameRuleEvents.hpp>

void AnimationEngine::operator()(const SetStateAnimationEvent&) {}

void AnimationEngine::operator()(const PlayerFiredAnimationEvent& e)
{
    std::cout << std::format(
        "{}: PlayerFiredAnimationEvent: {}", scene.tick, e.playerIdx);
    // TODO: set player state as well
    setWeaponAnimationState(
        getInventory(e.playerIdx), AnimationStateId::Missile);
}

void AnimationEngine::operator()(const WeaponSwappedAnimationEvent& e)
{
    setWeaponAnimationState(getInventory(e.playerIdx), AnimationStateId::Idle);
}

void AnimationEngine::update(const float)
{
    for (auto&& [thing, idx] : scene.things)
    {
        handleUpdate(thing.animationContext, thing.typeId, idx);
    }

    for (auto&& [_, inventory] : scene.playerStates)
    {
        handleUpdate(
            inventory.animationContext, inventory.activeWeaponType, -1);
    }
}

void AnimationEngine::handleUpdate(
    AnimationContext& context, EntityType entityType, std::size_t idx)
{
    const auto& eprop = ENTITY_PROPERTIES.at(entityType);
    if (eprop.states.empty()) return;

    auto& state = eprop.states.at(context.animationStateId);

    bool shouldUpdate =
        (scene.tick - context.lastAnimationUpdate) >= state.updateFrequency;
    if (!shouldUpdate) return;

    if ((context.animationFrameIndex + 1) == state.clip.size())
        handleTransition(context, entityType, idx, state);
    else
    {
        ++context.animationFrameIndex;
        updateSpriteId(context, state);
    }
}

void AnimationEngine::handleTransition(
    AnimationContext& context,
    EntityType entityType,
    std::size_t entityIdx,
    const AnimationState& oldState)
{
    switch (oldState.transition)
    {
        using enum AnimationStateId;

    case MarkerLoop:
        context.animationFrameIndex = 0;
        updateSpriteId(context, oldState);
        break;
    case MarkerDestroy:
        EventQueue::add<EntityDestroyedGameEvent>(entityIdx);
        break;
    case MarkerFreeze:
        // do nothing
        break;
    default: {
        // Switch to new state
        auto& newState =
            ENTITY_PROPERTIES.at(entityType).states.at(oldState.transition);
        context.animationStateId = oldState.transition;
        context.animationFrameIndex = 0;
        updateSpriteId(context, newState);
    }
    }
}

void AnimationEngine::setWeaponAnimationState(
    PlayerInventory& inventory, AnimationStateId state)
{
    inventory.animationContext.animationStateId = state;
    inventory.animationContext.animationFrameIndex = 0;
    updateSpriteId(
        inventory.animationContext,
        ENTITY_PROPERTIES.at(inventory.activeWeaponType)
            .states.at(inventory.animationContext.animationStateId));
}

void AnimationEngine::updateSpriteId(
    AnimationContext& context, const AnimationState& state)
{
    assert(state.clip.size() > context.animationFrameIndex);
    context.lastAnimationUpdate = scene.tick;
    context.spriteClipIndex = state.clip[context.animationFrameIndex];
}
