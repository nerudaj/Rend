#pragma once

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
    ExplosionF
};

enum class EntityType
{
    Marker,
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
    EffectExplosion,
};
