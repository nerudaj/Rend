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
    NoRender,
    FlagA = 1,
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
    PlayerA0 = 13,
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

    MarkerBegin,
    MarkerIdle,    // If animation reaches this mark, it should return to idle
                   // state
    MarkerLoop,    // If animation reaches this mark, it should loop the clip
    MarkerDestroy, // If animation reaches this mark, it should emit an event
                   // for destroying an item
    MarkerEnd
};

enum class EntityType
{
    MarkerBegin,
    MarkerDeadPlayer,
    MarkerEnd,

    StaticDecoration,
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
    EffectClip,
};

enum class AnimationId
{
    Idle,
    Run,
    Missile,
};
