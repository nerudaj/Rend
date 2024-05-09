#pragma once

#include <Literals.hpp>
#include <core/Enums.hpp>
#include <core/Scene.hpp>

struct [[nodiscard]] Script final
{
    ScriptId id = ScriptId::NoAction;
    const std::string sound = "";
    SoundSourceType soundSourceType = SoundSourceType::Ambient;
    EntityType entityType = EntityType::Error;
    int count = 0;
    int damage = 0;
};

struct [[nodiscard]] AnimationFrame final
{
    SpriteId spriteId = SpriteId::ErrorRender;
    std::size_t duration = 10; // in ticks
    Script scriptToTrigger;
};

using AnimationClip = std::vector<AnimationFrame>;

struct [[nodiscard]] AnimationState final
{
    AnimationClip clip;
    AnimationStateId transition = AnimationStateId::Idle;
};

using EntityStates = std::map<AnimationStateId, AnimationState>;

struct [[nodiscard]] EntityProperties final
{
    float radius = 0_px;
    float speed = 0.f;
    Trait traits = Trait::None;
    float explosionRadius = 0_px; // only considered for explosives
    const std::string specialSound =
        "";         // sounds related to game rules, like pickups or bounces
    int damage = 0; // only considered for projectiles
    AmmoType ammoType;
    int ammoAmount = 0;
    int minAmmoNeededToFire = 1;
    int ammoConsumedPerShot = 1;
    int healthAmount = 0;
    int armorAmount = 0;
    EntityType debrisEffectType = EntityType::None;
    SpriteId initialSpriteIndex;
    EntityStates states;
};
