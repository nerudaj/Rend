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
                                .clip = { { DeathA, 7 },
                                          { DeathB, 7 },
                                          { DeathC, 7 },
                                          { DeathD, 7 } },
                                .transition =
                                    AnimationStateId::MarkerFreeze } } } } },
        { EntityType::EffectExplosion,
          EntityProperties {
              .radius = 8_px,
              .initialSpriteIndex = ExplosionA,
              .states = { { AnimationStateId::Idle,
                            AnimationState {
                                .clip = { { ExplosionA, 2 },
                                          { ExplosionB, 2 },
                                          { ExplosionC, 2 },
                                          { ExplosionD, 2 },
                                          { ExplosionE, 2 },
                                          { ExplosionF, 2 } },
                                .transition =
                                    AnimationStateId::MarkerDestroy } } } } },
        { EntityType::EffectSpawn,
          EntityProperties {
              .radius = 8_px,
              .initialSpriteIndex = SpawnItemA,
              .states = { { AnimationStateId::Idle,
                            AnimationState {
                                .clip = { { SpawnItemA, 10 },
                                          { SpawnItemB, 10 },
                                          { SpawnItemC, 10 } },
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
        { EntityType::PickupTrishot,
          EntityProperties { .radius = 6_px, .initialSpriteIndex = TrishotA } },
        { EntityType::PickupCrossbow,
          EntityProperties { .radius = 6_px,
                             .initialSpriteIndex = CrossbowA } },
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
                                .clip = { { RocketA0, 10 }, { RocketB0, 10 } },
                                .transition =
                                    AnimationStateId::MarkerLoop } } } } },
        {
            EntityType::WeaponShotgun,
            EntityProperties {
                .radius = 0_px,
                .initialSpriteIndex = HUD_ShotgunA,
                .states = { { AnimationStateId::Idle,
                              AnimationState {
                                  .clip = { { HUD_ShotgunA, 10 } },
                                  .transition =
                                      AnimationStateId::MarkerLoop } },
                            { AnimationStateId::Missile,
                              AnimationState {
                                  .clip = { { HUD_ShotgunFA, 10 },
                                            { HUD_ShotgunFB, 10 },
                                            { HUD_ShotgunFA, 10 } },
                                  .transition = AnimationStateId::Recovery } },
                            { AnimationStateId::Recovery,
                              AnimationState {
                                  .clip = { { HUD_ShotgunRA, 10 },
                                            { HUD_ShotgunRB, 10 },
                                            { HUD_ShotgunRC, 10 } },
                                  .transition = AnimationStateId::Idle } } } },
        },
        {
            EntityType::WeaponFlaregun,
            EntityProperties {
                .radius = 0_px,
                .initialSpriteIndex = HUD_FlaregunA,
                .states = { { AnimationStateId::Idle,
                              AnimationState {
                                  .clip = { { HUD_FlaregunA, 10 } },
                                  .transition =
                                      AnimationStateId::MarkerLoop } },
                            { AnimationStateId::Missile,
                              AnimationState {
                                  .clip = { { HUD_FlaregunFA, 20 },
                                            { HUD_FlaregunFB, 20 } },
                                  .transition = AnimationStateId::Idle } } } },
        },
        {
            EntityType::WeaponTrishot,
            EntityProperties {
                .radius = 0_px,
                .initialSpriteIndex = HUD_TrishotA,
                .states = { { AnimationStateId::Idle,
                              AnimationState {
                                  .clip = { { HUD_TrishotA, 10 } },
                                  .transition =
                                      AnimationStateId::MarkerLoop } },
                            { AnimationStateId::Missile,
                              AnimationState {
                                  .clip = { { HUD_TrishotFA, 10 },
                                            { HUD_TrishotFB, 10 },
                                            { HUD_TrishotFA, 10 },
                                            { HUD_TrishotFB, 10 },
                                            { HUD_TrishotFA, 10 },
                                            { HUD_TrishotFB, 10 } },
                                  .transition = AnimationStateId::Idle } } } },
        },
        {
            EntityType::WeaponCrossbow,
            EntityProperties {
                .radius = 0_px,
                .initialSpriteIndex = HUD_CrossbowA,
                .states = { { AnimationStateId::Idle,
                              AnimationState {
                                  .clip = { { HUD_CrossbowA, 10 } },
                                  .transition =
                                      AnimationStateId::MarkerLoop } },
                            { AnimationStateId::Missile,
                              AnimationState {
                                  .clip = { { HUD_CrossbowFA, 5 },
                                            { HUD_CrossbowFB, 5 },
                                            { HUD_CrossbowFC, 5 },
                                            { HUD_CrossbowFD, 5 },
                                            { HUD_CrossbowFE, 5 },
                                            { HUD_CrossbowA, 10 } },
                                  .transition = AnimationStateId::Idle } } } },
        },
        {
            EntityType::Decal,
            EntityProperties {
                .radius = 1_px,
                .initialSpriteIndex = WallDecalA,
                .states = { { AnimationStateId::Idle,
                              AnimationState {
                                  .clip = { { WallDecalA, 10 },
                                            { WallDecalB, 10 } },
                                  .transition =
                                      AnimationStateId::MarkerDestroy } },
                            { AnimationStateId::Missile,
                              AnimationState {
                                  .clip = { { FleshDecalA, 10 },
                                            { FleshDecalB, 10 } },
                                  .transition =
                                      AnimationStateId::MarkerDestroy } } } },
        }
    };
}();