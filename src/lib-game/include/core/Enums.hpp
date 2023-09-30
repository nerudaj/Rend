#pragma once

#include <cstdint>

enum class LeveldItemId
{
    PlayerSpawn = 0,
    Flag,
    Medikit,
    ArmorShard,
    MegaHealth,
    MegaArmor,
    Bullets,
    Shells,
    EnergyPack,
    Rockets,
    Shotgun,
    Pillar,
    FloorLamp
};

enum class SpriteId : std::uint8_t
{
    NoRender = 0,
    FlagA = 1,
    FlagB,
    FlagC,
    MedikitA,
    ArmorShardA,
    MegaHealthA,
    MegaArmorA,
    BulletsA,
    ShellsA,
    EnergyPackA,
    RocketsA,
    ShotgunA,
    PillarA,
    FloorLampA,
    PlayerA0,
    PlayerA1,
    PlayerA2,
    PlayerA3,
    PlayerA4,
    PlayerB0,
    PlayerB1,
    PlayerB2,
    PlayerB3,
    PlayerB4,
    RocketA0,
    RocketA1,
    RocketA2,
    RocketA3,
    RocketA4,
    RocketB0,
    RocketB1,
    RocketB2,
    RocketB3,
    RocketB4,
    ExplosionA,
    ExplosionB,
    ExplosionC,
    ExplosionD,
    ExplosionE,
    ExplosionF,
    DeathA,
    DeathB,
    DeathC,
    DeathD,
    SpawnItemA,
    SpawnItemB,
    SpawnItemC,

    HUD_ShotgunA = 0,
    HUD_ShotgunFA,
    HUD_ShotgunFB
};

enum class EntityType
{
    Error,
    StaticDecorationBegin,
    Pillar,
    FloorLamp,
    StaticDecorationEnd,
    Player,

    PickableBegin,
    PickupHealth,
    PickupArmor,
    PickupMegaHealth,
    PickupMegaArmor,
    PickupBullets,
    PickupShells,
    PickupEnergy,
    PickupRockets,

    WeaponPickableBegin,
    PickupShotgun,
    WeaponPickableEnd,

    PickableEnd,

    ProjectileRocket,

    EffectBegin,
    EffectStatic, // Does nothing, has no collisions
    EffectDyingPlayer,
    EffectExplosion,
    EffectSpawn,
    EffectEnd,

    WeaponShotgun,
};

enum class AnimationStateId
{
    Idle,
    Run,
    Missile,
    MarkerLoop,
    MarkerDestroy,
    MarkerFreeze,
};
