#pragma once

#include <core/Enums.hpp>
#include <core/Scene.hpp>
#include <map>
#include <vector>

const static inline auto ENTITY_PROPERTIES =
    []() -> std::map<EntityType, EntityProperties>
{
    using enum SpriteId;
    return {
        { EntityType::EffectDyingPlayer,
          EntityProperties {
              .radius = 8_px,
              .initialSpriteIndex = DeathA,
              .states = { { AnimationStateId::Idle,
                            AnimationState {
                                .clip = { DeathA, DeathB, DeathC, DeathD },
                                .updateFrequency = 7,
                                .transition =
                                    AnimationStateId::MarkerFreeze } } } } },
        { EntityType::EffectExplosion,
          EntityProperties {
              .radius = 8_px,
              .initialSpriteIndex = ExplosionA,
              .states = { { AnimationStateId::Idle,
                            AnimationState {
                                .clip = { ExplosionA,
                                          ExplosionB,
                                          ExplosionC,
                                          ExplosionD,
                                          ExplosionE,
                                          ExplosionF },
                                .updateFrequency = 2,
                                .transition =
                                    AnimationStateId::MarkerDestroy } } } } },
        { EntityType::EffectSpawn,
          EntityProperties {
              .radius = 8_px,
              .initialSpriteIndex = SpawnItemA,
              .states = { { AnimationStateId::Idle,
                            AnimationState {
                                .clip = { SpawnItemA, SpawnItemB, SpawnItemC },
                                .updateFrequency = 10,
                                .transition =
                                    AnimationStateId::MarkerDestroy } } } } },
        { EntityType::Player,
          EntityProperties { .radius = 4_px, .initialSpriteIndex = PlayerA0 } },
        { EntityType::PickupHealth,
          EntityProperties { .radius = 3_px, .initialSpriteIndex = MedikitA } },
        { EntityType::PickupArmor,
          EntityProperties { .radius = 3_px,
                             .initialSpriteIndex = ArmorShardA } },
        { EntityType::PickupMegaHealth,
          EntityProperties { .radius = 6_px,
                             .initialSpriteIndex = MegaHealthA } },
        { EntityType::PickupMegaArmor,
          EntityProperties { .radius = 3_px,
                             .initialSpriteIndex = MegaArmorA } },
        { EntityType::PickupBullets,
          EntityProperties { .radius = 4_px, .initialSpriteIndex = BulletsA } },
        { EntityType::PickupShells,
          EntityProperties { .radius = 3_px, .initialSpriteIndex = ShellsA } },
        { EntityType::PickupEnergy,
          EntityProperties { .radius = 4_px,
                             .initialSpriteIndex = EnergyPackA } },
        { EntityType::PickupRockets,
          EntityProperties { .radius = 3_px, .initialSpriteIndex = RocketsA } },
        { EntityType::PickupShotgun,
          EntityProperties { .radius = 6_px, .initialSpriteIndex = ShotgunA } },
        { EntityType::Pillar,
          EntityProperties { .radius = 3_px, .initialSpriteIndex = PillarA } },
        { EntityType::FloorLamp,
          EntityProperties { .radius = 3_px,
                             .initialSpriteIndex = FloorLampA } },
        { EntityType::ProjectileRocket,
          EntityProperties {
              .radius = 2_px,
              .initialSpriteIndex = RocketA0,
              .states = { { AnimationStateId::Idle,
                            AnimationState {
                                .clip = { RocketA0, RocketB0 },
                                .updateFrequency = 10,
                                .transition =
                                    AnimationStateId::MarkerLoop } } } } },
        {
            EntityType::WeaponShotgun,
            EntityProperties {
                .radius = 0_px,
                .initialSpriteIndex = HUD_ShotgunA,
                .states = { { AnimationStateId::Idle,
                              AnimationState {
                                  .clip = { HUD_ShotgunA },
                                  .updateFrequency = 10,
                                  .transition =
                                      AnimationStateId::MarkerLoop } },
                            { AnimationStateId::Missile,
                              AnimationState {
                                  .clip = { HUD_ShotgunFA,
                                            HUD_ShotgunFB,
                                            HUD_ShotgunFA },
                                  .updateFrequency = 5,
                                  .transition = AnimationStateId::Idle } } } },
        }
    };
}();