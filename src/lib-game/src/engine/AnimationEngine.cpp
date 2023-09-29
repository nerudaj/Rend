#include <core/Constants.hpp>
#include <engine/AnimationEngine.hpp>
#include <events/EventQueue.hpp>
#include <events/GameRuleEvents.hpp>

#pragma region AnimationDefinitions
// clang-format off
const auto SPAWN_ITEM_CLIP = AnimationClip { SpriteId::SpawnItemA, SpriteId::SpawnItemB, SpriteId::SpawnItemC };

using EntityStates = std::map<AnimationStateId, AnimationState>;
const auto STATES = [] () -> std::map<EntityType, EntityStates> {
    using enum SpriteId;
    return {
        {
            EntityType::EffectExplosion, 
            EntityStates
            {
                {
                    AnimationStateId::Idle,
                    AnimationState {
                        .clip = { ExplosionA, ExplosionB, ExplosionC, ExplosionD, ExplosionE, ExplosionF },
                        .updateFrequency = 2,
                        .transition = AnimationStateId::MarkerDestroy
                    }
                }
            }
        },
        {
            EntityType::EffectDyingPlayer,
            EntityStates
            {
                {
                    AnimationStateId::Idle,
                    AnimationState {
                        .clip = { DeathA, DeathB, DeathC, DeathD },
                        .updateFrequency = 7,
                        .transition = AnimationStateId::MarkerFreeze
                    }
                }
            }
        },
        {
            EntityType::EffectSpawn,
            EntityStates
            {
                {
                    AnimationStateId::Idle,
                    AnimationState {
                        .clip = { SpawnItemA, SpawnItemB, SpawnItemC },
                        .updateFrequency = 10,
                        .transition = AnimationStateId::MarkerDestroy
                    }
                }
            }
        },
        {
            EntityType::ProjectileRocket,
            EntityStates
            {
                {
                    AnimationStateId::Idle,
                    AnimationState {
                        .clip = { RocketA0, RocketB0 },
                        .updateFrequency = 4,
                        .transition = AnimationStateId::MarkerLoop
                    }
                }
            }
        }
    };
} ();
// clang-format on
#pragma endregion

void AnimationEngine::operator()(const SetStateAnimationEvent&) {}

void AnimationEngine::update(const float)
{
    for (auto&& [thing, idx] : scene.things)
    {
        if (!STATES.contains(thing.typeId)) continue;

        auto& state = STATES.at(thing.typeId).at(thing.animationStateId);

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
        auto& newState = STATES.at(entity.typeId).at(oldState.transition);
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
