#pragma once

#include <core/Enums.hpp>
#include <core/Scene.hpp>

struct Script
{
    ScriptId id = ScriptId::NoAction;
    const std::string sound = "";
    SoundSourceType soundSourceType = SoundSourceType::Ambient;

    constexpr Script() = default;

    constexpr Script(ScriptId id) : id(id) {}

    constexpr Script(
        const std::string& sound,
        SoundSourceType type = SoundSourceType::Ambient)
        : id(ScriptId::TriggerSound), sound(sound), soundSourceType(type)
    {
    }

    constexpr Script(
        ScriptId id,
        const std::string& sound,
        SoundSourceType type = SoundSourceType::Ambient)
        : id(id), sound(sound), soundSourceType(type)
    {
    }
};

struct AnimationFrame
{
    SpriteId spriteId = SpriteId::ErrorRender;
    std::size_t duration = 10; // in ticks
    Script scriptToTrigger;
};

using AnimationClip = std::vector<AnimationFrame>;

struct AnimationState
{
    AnimationClip clip;
    AnimationStateId transition;
};

using EntityStates = std::map<AnimationStateId, AnimationState>;

struct EntityProperties
{
    float radius;
    float speed = 0.f;
    bool isExplosive = false;
    float explosionRadius = 0_px;
    int damage = 0;
    AmmoType ammoType;
    int ammoAmount = 0;
    int healthAmount = 0;
    int armorAmount = 0;
    EntityType debrisEffectType = EntityType::None;
    SpriteId initialSpriteIndex;
    EntityStates states;
};
