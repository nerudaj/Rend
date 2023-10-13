#include <core/Constants.hpp>
#include <core/EntityDefinitions.hpp>
#include <engine/AnimationEngine.hpp>
#include <events/EventQueue.hpp>
#include <events/GameRuleEvents.hpp>

void AnimationEngine::operator()(const SetStateAnimationEvent&) {}

void AnimationEngine::operator()(const PlayerFiredAnimationEvent& e)
{
#ifdef DEBUG_REMOVALS 1

    std::cout << std::format(
        "{}: playerFired(idx = {})", scene.tick, e.playerIdx)
              << std::endl;

#endif

    // TODO: set player state as well
    setWeaponAnimationState(e.playerIdx, AnimationStateId::Missile);
}

void AnimationEngine::operator()(const WeaponSwappedAnimationEvent& e)
{
    setWeaponAnimationState(e.playerIdx, AnimationStateId::Idle);
}

void AnimationEngine::update(const float)
{
    for (auto&& [thing, idx] : scene.things)
    {
        handleUpdate(thing.animationContext, thing.typeId, idx);

        if (thing.typeId == EntityType::Player)
        {
            auto& inventory = scene.playerStates[thing.stateId].inventory;
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
    std::size_t entityIdx,
    const AnimationState& oldState)
{
    switch (oldState.transition)
    {
        using enum AnimationStateId;

    case MarkerLoop:
        context.animationFrameIndex = 0;
        updateSpriteId(context, oldState, entityIdx);
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
        updateSpriteId(context, newState, entityIdx);
    }
    }
}

void AnimationEngine::setWeaponAnimationState(
    EntityIndexType playerIdx, AnimationStateId state)
{
    auto&& inventory = getInventory(playerIdx);
    inventory.animationContext.animationStateId = state;
    inventory.animationContext.animationFrameIndex = 0;
    updateSpriteId(
        inventory.animationContext,
        ENTITY_PROPERTIES.at(inventory.activeWeaponType)
            .states.at(inventory.animationContext.animationStateId),
        playerIdx);
}

void AnimationEngine::updateSpriteId(
    AnimationContext& context, const AnimationState& state, EntityIndexType idx)
{
    assert(state.clip.size() > context.animationFrameIndex);
    context.lastAnimationUpdate = scene.tick;
    context.spriteClipIndex = state.clip[context.animationFrameIndex].spriteId;

    const auto script = state.clip[context.animationFrameIndex].scriptToTrigger;
    if (script != ScriptId::NoAction)
        EventQueue::add<ScriptTriggeredGameEvent>(
            ScriptTriggeredGameEvent(script, idx));
}
