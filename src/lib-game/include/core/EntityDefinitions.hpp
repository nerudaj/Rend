#pragma once

#include <Literals.hpp>
#include <core/EntityDefinitionTypes.hpp>
#include <map>
#include <utils/SemanticTypes.hpp>
#include <vector>

constinit const unsigned LOWER_TIME = 30;
constinit const unsigned RAISE_TIME = 30;
constinit const unsigned FAST_LOWER_TIME = 10;
constinit const unsigned FAST_RAISE_TIME = 10;

constinit const int FLARE_DAMAGE = 64_damage;
constinit const int SHOTGUN_DAMAGE = 16_damage;
constinit const int BULLET_DAMAGE = 20_damage;
constinit const int LASERDART_DAMAGE = 79_damage;
constinit const int ROCKET_DAMAGE = 150_damage;
constinit const int RAILGUN_DAMAGE = 200_damage;

constinit const int BULLET_AMMO_PICKUP = 18;
constinit const int SHELL_AMMO_PICKUP = 6;
constinit const int ENERGY_AMMO_PICKUP = 6;
constinit const int ROCKET_AMMO_PICKUP = 2;

constexpr Script playSound(
    const std::string& sound, SoundSourceType type = SoundSourceType::Ambient)
{
    return Script { .id = ScriptId::TriggerSound,
                    .sound = sound,
                    .soundSourceType = type };
}

constexpr Script
fireProjectile(EntityType projectileType, const std::string& sound)
{
    return Script { .id = ScriptId::FireProjectile,
                    .sound = sound,
                    .soundSourceType = SoundSourceType::Player,
                    .entityType = projectileType };
}

constexpr Script firePellets(int count, int damage, const std::string& sound)
{
    return Script { .id = ScriptId::FirePellets,
                    .sound = sound,
                    .soundSourceType = SoundSourceType::Player,
                    .count = count,
                    .damage = damage };
}

constexpr Script fireBullet(int damage, const std::string& sound)
{
    return Script { .id = ScriptId::FireBullet,
                    .sound = sound,
                    .soundSourceType = SoundSourceType::Player,
                    .damage = damage };
}

constexpr Script fireRay(int damage, const std::string& sound)
{
    return Script { .id = ScriptId::FireRay,
                    .sound = sound,
                    .soundSourceType = SoundSourceType::Player,
                    .damage = damage };
}

const static inline auto ENTITY_PROPERTIES =
    []() -> std::map<EntityType, EntityProperties>
{
    using enum SpriteId;
    return {
        { EntityType::CameraAnchor,
          EntityProperties { .radius = 1_px, .initialSpriteIndex = NoRender } },
        { EntityType::EffectDyingPlayer,
          EntityProperties {
              .radius = 8_px,
              .initialSpriteIndex = DeathA,
              .states = { { AnimationStateId::Idle,
                            AnimationState {
                                .clip = { { DeathA, 7 },
                                          { DeathB, 7 },
                                          { DeathC, 7 },
                                          { DeathD, 600 } },
                                .transition =
                                    AnimationStateId::MarkerDestroy } } } } },
        { EntityType::EffectFlareExplosion,
          EntityProperties {
              .radius = 8_px,
              .initialSpriteIndex = FlareExplosionA,
              .states = { { AnimationStateId::Idle,
                            AnimationState {
                                .clip = { { FlareExplosionA,
                                            1 }, // Padding required before
                                                 // sound playback
                                          { FlareExplosionA,
                                            6,
                                            playSound("flare_explosion.wav") },
                                          { FlareExplosionB, 7 },
                                          { FlareExplosionC, 7 },
                                          { FlareExplosionD, 7 },
                                          { FlareExplosionE, 7 },
                                          { FlareExplosionF, 300 } },
                                .transition =
                                    AnimationStateId::MarkerDestroy } } } } },
        { EntityType::EffectRocketExplosion,
          EntityProperties {
              .radius = 8_px,
              .initialSpriteIndex = ExplosionA,
              .states = { { AnimationStateId::Idle,
                            AnimationState {
                                .clip = { { ExplosionA,
                                            1 }, // Padding required before
                                                 // sound playback
                                          { ExplosionA,
                                            1,
                                            playSound("rocket_explosion.wav") },
                                          { ExplosionB, 2 },
                                          { ExplosionC, 2 },
                                          { ExplosionD, 2 },
                                          { ExplosionE, 2 },
                                          { ExplosionF, 2 } },
                                .transition =
                                    AnimationStateId::MarkerDestroy } } } } },
        { EntityType::EffectDartExplosion,
          EntityProperties {
              .radius = 4_px,
              .initialSpriteIndex = DartExplosionA,
              .states = { { AnimationStateId::Idle,
                            AnimationState {
                                .clip = { { DartExplosionA,
                                            1 }, // Padding required before
                                                 // sound playback
                                          { DartExplosionA,
                                            2,
                                            playSound("dart_explosion.wav") },
                                          { DartExplosionB, 3 },
                                          { DartExplosionC, 3 } },
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
        { EntityType::EffectRailDecal,
          EntityProperties {
              .radius = 4_px,
              .initialSpriteIndex = RailDecalA,
              .states = { { AnimationStateId::Idle,
                            AnimationState {
                                .clip = { { RailDecalA, 10 },
                                          { RailDecalB, 10 },
                                          { RailDecalC, 10 } },
                                .transition =
                                    AnimationStateId::MarkerDestroy } } } } },
        { EntityType::Player,
          EntityProperties {
              .radius = 4_px,
              .speed = 96_unitspersec,
              .traits = Trait::Solid | Trait::Destructible | Trait::Directional,
              .debrisEffectType = EntityType::EffectDyingPlayer,
              .initialSpriteIndex = PlayerA0,
              .states = { { AnimationStateId::Idle,
                            AnimationState {
                                .clip = { { PlayerA0, 10 } },
                                .transition = AnimationStateId::MarkerLoop } },
                          { AnimationStateId::Run,
                            AnimationState {
                                .clip = { { PlayerA0, 10 }, { PlayerB0, 10 } },
                                .transition = AnimationStateId::Idle } },
                          { AnimationStateId::Missile,
                            AnimationState {
                                .clip = { { PlayerFA0, 10 },
                                          { PlayerFB0, 10 } },
                                .transition = AnimationStateId::Idle } } } } },
        { EntityType::PickupHealth,
          EntityProperties { .radius = 3_px,
                             .traits = Trait::Pickable,
                             .specialSound = "pickup.wav",
                             .healthAmount = 25,
                             .initialSpriteIndex = MedikitA } },
        { EntityType::PickupArmor,
          EntityProperties { .radius = 3_px,
                             .traits = Trait::Pickable,
                             .specialSound = "pickup.wav",
                             .armorAmount = 10,
                             .initialSpriteIndex = ArmorShardA } },
        { EntityType::PickupMegaHealth,
          EntityProperties { .radius = 6_px,
                             .traits = Trait::Pickable | Trait::PowerItem,
                             .specialSound = "megapickup.wav",
                             .healthAmount = 100,
                             .initialSpriteIndex = MegaHealthA } },
        { EntityType::PickupMegaArmor,
          EntityProperties { .radius = 3_px,
                             .traits = Trait::Pickable | Trait::PowerItem,
                             .specialSound = "megapickup.wav",
                             .armorAmount = 100,
                             .initialSpriteIndex = MegaArmorA } },
        { EntityType::PickupBullets,
          EntityProperties { .radius = 4_px,
                             .traits = Trait::Pickable,
                             .specialSound = "pickup.wav",
                             .ammoAmount = BULLET_AMMO_PICKUP,
                             .initialSpriteIndex = BulletsA } },
        { EntityType::PickupShells,
          EntityProperties { .radius = 3_px,
                             .traits = Trait::Pickable,
                             .specialSound = "pickup.wav",
                             .ammoAmount = SHELL_AMMO_PICKUP,
                             .initialSpriteIndex = ShellsA } },
        { EntityType::PickupEnergy,
          EntityProperties { .radius = 4_px,
                             .traits = Trait::Pickable,
                             .specialSound = "pickup.wav",
                             .ammoAmount = ENERGY_AMMO_PICKUP,
                             .initialSpriteIndex = EnergyPackA } },
        { EntityType::PickupRockets,
          EntityProperties { .radius = 3_px,
                             .traits = Trait::Pickable,
                             .specialSound = "pickup.wav",
                             .ammoAmount = ROCKET_AMMO_PICKUP,
                             .initialSpriteIndex = RocketsA } },
        { EntityType::PickupShotgun,
          EntityProperties { .radius = 6_px,
                             .traits = Trait::Pickable | Trait::WeaponPickup,
                             .specialSound = "pickup_weapon.wav",
                             .ammoType = AmmoType::Shells,
                             .initialSpriteIndex = ShotgunA } },
        { EntityType::PickupTrishot,
          EntityProperties { .radius = 6_px,
                             .traits = Trait::Pickable | Trait::WeaponPickup,
                             .specialSound = "pickup_weapon.wav",
                             .ammoType = AmmoType::Bullets,
                             .initialSpriteIndex = TrishotA } },
        { EntityType::PickupCrossbow,
          EntityProperties { .radius = 6_px,
                             .traits = Trait::Pickable | Trait::WeaponPickup,
                             .specialSound = "pickup_weapon.wav",
                             .ammoType = AmmoType::Energy,
                             .initialSpriteIndex = CrossbowA } },
        { EntityType::PickupLauncher,
          EntityProperties { .radius = 6_px,
                             .traits = Trait::Pickable | Trait::WeaponPickup,
                             .specialSound = "pickup_weapon.wav",
                             .ammoType = AmmoType::Rockets,
                             .initialSpriteIndex = LauncherA } },
        { EntityType::PickupBallista,
          EntityProperties { .radius = 6_px,
                             .traits = Trait::Pickable | Trait::WeaponPickup,
                             .specialSound = "pickup_weapon.wav",
                             .ammoType = AmmoType::Energy,
                             .initialSpriteIndex = BallistaA } },
        { EntityType::Pillar,
          EntityProperties { .radius = 3_px,
                             .traits = Trait::Solid,
                             .initialSpriteIndex = PillarA } },
        { EntityType::FloorLamp,
          EntityProperties { .radius = 3_px,
                             .traits = Trait::Solid,
                             .initialSpriteIndex = FloorLampA } },
        { EntityType::CeilLamp,
          EntityProperties { .radius = 4_px,
                             .initialSpriteIndex = CeilLampA } },
        { EntityType::ProjectileFlare,
          EntityProperties {
              .radius = 2_px,
              .speed = 200_unitspersec,
              .traits = Trait::Projectile,
              .damage = FLARE_DAMAGE,
              .debrisEffectType = EntityType::EffectFlareExplosion,
              .initialSpriteIndex = FlareA0,
          } },
        { EntityType::ProjectileRocket,
          EntityProperties {
              .radius = 2_px,
              .speed = 150_unitspersec,
              .traits =
                  Trait::Projectile | Trait::Explosive | Trait::Directional,
              .explosionRadius = 16_px,
              .damage = ROCKET_DAMAGE,
              .debrisEffectType = EntityType::EffectRocketExplosion,
              .initialSpriteIndex = RocketA0,
              .states = { { AnimationStateId::Idle,
                            AnimationState {
                                .clip = { { RocketA0, 10 }, { RocketB0, 10 } },
                                .transition =
                                    AnimationStateId::MarkerLoop } } } } },
        { EntityType::ProjectileLaserDart,
          EntityProperties {
              .radius = 2_px,
              .speed = 150_unitspersec,
              .traits = Trait::Projectile | Trait::Bouncy | Trait::Directional,
              .specialSound = "dart_bounce.wav",
              .damage = LASERDART_DAMAGE,
              .debrisEffectType = EntityType::EffectDartExplosion,
              .initialSpriteIndex = LaserDartA0,
          } },
        {
            EntityType::WeaponFlaregun,
            EntityProperties {
                .radius = 0_px,
                .ammoType = AmmoType::Rockets,
                .ammoAmount = 30,
                .minAmmoNeededToFire = 1,
                .ammoConsumedPerShot = 1,
                .initialSpriteIndex = HUD_FlaregunA,
                .states = { { AnimationStateId::Idle,
                              AnimationState {
                                  .clip = { { HUD_FlaregunA, 10 } },
                                  .transition =
                                      AnimationStateId::MarkerLoop } },
                            { AnimationStateId::Missile,
                              AnimationState {
                                  .clip = { { HUD_FlaregunFA, 10 },
                                            { HUD_FlaregunFA,
                                              10,
                                              fireProjectile(
                                                  EntityType::ProjectileFlare,
                                                  "flaregun_fire.wav") },
                                            { HUD_FlaregunFB, 20 } },
                                  .transition = AnimationStateId::Recovery } },
                            { AnimationStateId::Recovery,
                              AnimationState {
                                  .clip = { { HUD_FlaregunA, 10 } },
                                  .transition = AnimationStateId::Idle } },
                            { AnimationStateId::Lower,
                              AnimationState {
                                  .clip = { { HUD_FlaregunA, LOWER_TIME } },
                                  .transition = AnimationStateId::Raise } },
                            { AnimationStateId::Raise,
                              AnimationState {
                                  .clip = { { HUD_FlaregunA, RAISE_TIME } } } },
                            { AnimationStateId::FastLower,
                              AnimationState {
                                  .clip = { { HUD_FlaregunA,
                                              FAST_LOWER_TIME } },
                                  .transition = AnimationStateId::FastRaise } },
                            { AnimationStateId::FastRaise,
                              AnimationState {
                                  .clip = { { HUD_FlaregunA,
                                              FAST_RAISE_TIME } } } } } },
        },
        {
            EntityType::WeaponShotgun,
            EntityProperties {
                .radius = 0_px,
                .ammoType = AmmoType::Shells,
                .ammoAmount = 20,
                .minAmmoNeededToFire = 1,
                .ammoConsumedPerShot = 1,
                .initialSpriteIndex = HUD_ShotgunA,
                .states = { { AnimationStateId::Idle,
                              AnimationState {
                                  .clip = { { HUD_ShotgunA, 10 } },
                                  .transition =
                                      AnimationStateId::MarkerLoop } },
                            { AnimationStateId::Missile,
                              AnimationState {
                                  .clip = { { HUD_ShotgunFA, 10 },
                                            { HUD_ShotgunFB,
                                              10,
                                              firePellets(
                                                  8,
                                                  SHOTGUN_DAMAGE,
                                                  "shotgun.wav") },
                                            { HUD_ShotgunFA, 10 } },
                                  .transition = AnimationStateId::Recovery } },
                            { AnimationStateId::Recovery,
                              AnimationState {
                                  .clip = { { HUD_ShotgunRA,
                                              15,
                                              playSound(
                                                  "shotgun_reload.wav",
                                                  SoundSourceType::Pov) },
                                            { HUD_ShotgunRB, 15 },
                                            { HUD_ShotgunRC, 15 } },
                                  .transition = AnimationStateId::Idle } },
                            { AnimationStateId::Lower,
                              AnimationState {
                                  .clip = { { HUD_ShotgunA, LOWER_TIME } },
                                  .transition = AnimationStateId::Raise } },
                            { AnimationStateId::Raise,
                              AnimationState {
                                  .clip = { { HUD_ShotgunA, RAISE_TIME } } } },
                            { AnimationStateId::FastLower,
                              AnimationState {
                                  .clip = { { HUD_ShotgunA, FAST_LOWER_TIME } },
                                  .transition = AnimationStateId::FastRaise } },
                            { AnimationStateId::FastRaise,
                              AnimationState {
                                  .clip = { { HUD_ShotgunA,
                                              FAST_RAISE_TIME } } } } } },
        },
        {
            EntityType::WeaponTrishot,
            EntityProperties {
                .radius = 0_px,
                .ammoType = AmmoType::Bullets,
                .ammoAmount = 50,
                .minAmmoNeededToFire = 3,
                .ammoConsumedPerShot = 1,
                .initialSpriteIndex = HUD_TrishotA,
                .states = { { AnimationStateId::Idle,
                              AnimationState {
                                  .clip = { { HUD_TrishotA, 10 } },
                                  .transition =
                                      AnimationStateId::MarkerLoop } },
                            { AnimationStateId::Missile,
                              AnimationState {
                                  .clip = { { HUD_TrishotFA,
                                              3,
                                              fireBullet(
                                                  BULLET_DAMAGE,
                                                  "bullet.wav") },
                                            { HUD_TrishotFB, 3 },
                                            { HUD_TrishotFC, 3 },
                                            { HUD_TrishotFD, 3 },
                                            { HUD_TrishotFA,
                                              3,
                                              fireBullet(
                                                  BULLET_DAMAGE,
                                                  "bullet.wav") },
                                            { HUD_TrishotFB, 3 },
                                            { HUD_TrishotFC, 3 },
                                            { HUD_TrishotFD, 3 },
                                            { HUD_TrishotFA,
                                              3,
                                              fireBullet(
                                                  BULLET_DAMAGE,
                                                  "bullet.wav") },
                                            { HUD_TrishotFB, 3 },
                                            { HUD_TrishotFC, 3 },
                                            { HUD_TrishotFD,
                                              3,
                                              ScriptId::ReleaseTrigger } },
                                  .transition = AnimationStateId::Idle } },
                            { AnimationStateId::Lower,
                              AnimationState {
                                  .clip = { { HUD_TrishotA, LOWER_TIME } },
                                  .transition = AnimationStateId::Raise } },
                            { AnimationStateId::Raise,
                              AnimationState {
                                  .clip = { { HUD_TrishotA, RAISE_TIME } } } },
                            { AnimationStateId::FastLower,
                              AnimationState {
                                  .clip = { { HUD_TrishotA, FAST_LOWER_TIME } },
                                  .transition = AnimationStateId::FastRaise } },
                            { AnimationStateId::FastRaise,
                              AnimationState {
                                  .clip = { { HUD_TrishotA,
                                              FAST_RAISE_TIME } } } } } },
        },
        {
            EntityType::WeaponCrossbow,
            EntityProperties {
                .radius = 0_px,
                .ammoType = AmmoType::Energy,
                .ammoAmount = 10,
                .minAmmoNeededToFire = 1,
                .ammoConsumedPerShot = 1,
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
                                            { HUD_CrossbowFC,
                                              5,
                                              fireProjectile(
                                                  EntityType::
                                                      ProjectileLaserDart,
                                                  "lasercrossbow_fire.wav") },
                                            { HUD_CrossbowFD, 5 },
                                            { HUD_CrossbowFE, 5 },
                                            { HUD_CrossbowA, 10 } },
                                  .transition = AnimationStateId::Idle } },
                            { AnimationStateId::Lower,
                              AnimationState {
                                  .clip = { { HUD_CrossbowA, LOWER_TIME } },
                                  .transition = AnimationStateId::Raise } },
                            { AnimationStateId::Raise,
                              AnimationState {
                                  .clip = { { HUD_CrossbowA, RAISE_TIME } } } },
                            { AnimationStateId::FastLower,
                              AnimationState {
                                  .clip = { { HUD_CrossbowA,
                                              FAST_LOWER_TIME } },
                                  .transition = AnimationStateId::FastRaise } },
                            { AnimationStateId::FastRaise,
                              AnimationState {
                                  .clip = { { HUD_CrossbowA,
                                              FAST_RAISE_TIME } } } } } },
        },
        {
            EntityType::WeaponLauncher,
            EntityProperties {
                .radius = 0_px,
                .ammoType = AmmoType::Rockets,
                .ammoAmount = 10,
                .minAmmoNeededToFire = 2,
                .ammoConsumedPerShot = 2,
                .initialSpriteIndex = HUD_LauncherA,
                .states = { { AnimationStateId::Idle,
                              AnimationState {
                                  .clip = { { HUD_LauncherA, 10 } },
                                  .transition =
                                      AnimationStateId::MarkerLoop } },
                            { AnimationStateId::Missile,
                              AnimationState {
                                  .clip = { { HUD_LauncherFA,
                                              10,
                                              fireProjectile(
                                                  EntityType::ProjectileRocket,
                                                  "launcher_fire.wav") },
                                            { HUD_LauncherFB, 10 },
                                            { HUD_LauncherFC, 10 },
                                            { HUD_LauncherFD,
                                              10,
                                              ScriptId::ReleaseTrigger } },
                                  .transition = AnimationStateId::Idle } },
                            { AnimationStateId::Lower,
                              AnimationState {
                                  .clip = { { HUD_LauncherA, LOWER_TIME } },
                                  .transition = AnimationStateId::Raise } },
                            { AnimationStateId::Raise,
                              AnimationState {
                                  .clip = { { HUD_LauncherA, RAISE_TIME } } } },
                            { AnimationStateId::FastLower,
                              AnimationState {
                                  .clip = { { HUD_LauncherA,
                                              FAST_LOWER_TIME } },
                                  .transition = AnimationStateId::FastRaise } },
                            { AnimationStateId::FastRaise,
                              AnimationState {
                                  .clip = { { HUD_LauncherA,
                                              FAST_RAISE_TIME } } } } } },
        },
        {
            EntityType::WeaponBallista,
            EntityProperties {
                .radius = 0_px,
                .ammoType = AmmoType::Energy,
                .ammoAmount = 20,
                .minAmmoNeededToFire = 3,
                .ammoConsumedPerShot = 3,
                .initialSpriteIndex = HUD_BallistaA,
                .states = { { AnimationStateId::Idle,
                              AnimationState {
                                  .clip = { { HUD_BallistaA, 10 },
                                            { HUD_BallistaB, 10 },
                                            { HUD_BallistaC, 10 } },
                                  .transition =
                                      AnimationStateId::MarkerLoop } },
                            { AnimationStateId::Missile,
                              AnimationState {
                                  .clip = { { HUD_BallistaFA,
                                              4,
                                              playSound(
                                                  "ballista_powering.wav",
                                                  SoundSourceType::Player) },
                                            { HUD_BallistaFB, 4 },
                                            { HUD_BallistaFC, 4 },
                                            { HUD_BallistaFD, 4 },
                                            { HUD_BallistaFE, 4 },
                                            { HUD_BallistaFF, 4 },
                                            { HUD_BallistaFG,
                                              5,
                                              fireRay(
                                                  RAILGUN_DAMAGE,
                                                  "railgun.wav") } },
                                  .transition = AnimationStateId::Recovery } },
                            { AnimationStateId::Recovery,
                              AnimationState {
                                  .clip = { { HUD_BallistaRA, 10 },
                                            { HUD_BallistaRB, 10 },
                                            { HUD_BallistaRC, 10 },
                                            { HUD_BallistaRD, 10 },
                                            { HUD_BallistaRE, 10 } },
                                  .transition = AnimationStateId::Idle } },
                            { AnimationStateId::Lower,
                              AnimationState {
                                  .clip = { { HUD_BallistaA, LOWER_TIME } },
                                  .transition = AnimationStateId::Raise } },
                            { AnimationStateId::Raise,
                              AnimationState {
                                  .clip = { { HUD_BallistaA, RAISE_TIME } } } },
                            { AnimationStateId::FastLower,
                              AnimationState {
                                  .clip = { { HUD_BallistaA,
                                              FAST_LOWER_TIME } },
                                  .transition = AnimationStateId::FastRaise } },
                            { AnimationStateId::FastRaise,
                              AnimationState {
                                  .clip = { { HUD_BallistaA,
                                              FAST_RAISE_TIME } } } } } },
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