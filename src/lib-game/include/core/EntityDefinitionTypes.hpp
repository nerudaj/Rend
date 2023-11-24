#pragma once

#include <core/Enums.hpp>
#include <core/Scene.hpp>

struct Script
{
    ScriptId id = ScriptId::NoAction;
    const std::string sound = "";
    SoundSourceType soundSourceType = SoundSourceType::Ambient;
    EntityType entityType = EntityType::Error;
    int count = 0;
    int damage = 0;
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
    float radius = 0_px;
    float speed = 0.f;
    bool isExplosive = false;
    bool isBouncy = false;        // only considered for projectiles
    float explosionRadius = 0_px; // only considered for explosives
    const std::string specialSound =
        "";         // sounds related to game rules, like pickups or bounces
    int damage = 0; // only considered for projectiles
    AmmoType ammoType;
    int ammoAmount = 0;
    int healthAmount = 0;
    int armorAmount = 0;
    EntityType debrisEffectType = EntityType::None;
    SpriteId initialSpriteIndex;
    EntityStates states;
};
