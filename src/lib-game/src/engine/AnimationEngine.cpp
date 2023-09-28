#include <core/Constants.hpp>
#include <engine/AnimationEngine.hpp>
#include <events/EventQueue.hpp>

using AnimationClip = std::vector<SpriteId>;

static std::map<SpriteId, SpriteId>
buildAnimationTransitions(const std::vector<AnimationClip>& clips)
{
    using enum SpriteId;

    std::map<SpriteId, SpriteId> result;

    for (auto&& clip : clips)
    {
        for (unsigned i = 0; i < clip.size() - 1; i++)
        {
            result[clip[i]] = clip[i + 1];
        }
    }

    return result;
}

using enum SpriteId;
const auto&& ANIMATION_TRANSITIONS =
    buildAnimationTransitions({ { ExplosionA,
                                  ExplosionB,
                                  ExplosionC,
                                  ExplosionD,
                                  ExplosionE,
                                  ExplosionF,
                                  MarkerDestroy },
                                { RocketA0, RocketB0, RocketA0 },
                                { DeathA, DeathB, DeathC, DeathD, DeathD } });

const std::map<std::pair<AnimationId, EntityType>, SpriteId>
    STATE_TRANSITIONS = { { { AnimationId::Idle, EntityType::Player },
                            SpriteId::PlayerA0 } };

[[nodiscard]] static constexpr inline bool isAnimationMarker(SpriteId id)
{
    return SpriteId::MarkerBegin <= id && id <= SpriteId::MarkerEnd;
}

// ======================

void AnimationEngine::operator()(const SetStateAnimationEvent&) {}

void AnimationEngine::update(const dgm::Time&)
{
    bool shouldUpdate = scene.frameId % ANIMATION_FPS == 0;
    if (!shouldUpdate) return;

    for (auto&& [thing, index] : scene.things)
    {
        if (!isAnimable(thing.typeId)) continue;

        thing.spriteClipIndex = ANIMATION_TRANSITIONS.at(thing.spriteClipIndex);

        if (isAnimationMarker(thing.spriteClipIndex))
            handleMarker(thing, index);
    }
}

void AnimationEngine::handleMarker(Entity& entity, std::size_t index)
{
    if (entity.spriteClipIndex == SpriteId::MarkerIdle)
    {
        entity.spriteClipIndex =
            STATE_TRANSITIONS.at({ AnimationId::Idle, entity.typeId });
    }
    else if (entity.spriteClipIndex == SpriteId::MarkerDestroy)
    {
        EventQueue::add<EntityDestroyedGameEvent>(index);
    }
}
