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
    Trishot,
    Crossbow,
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
    TrishotA,
    CrossbowA,
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
    LaserDartA0,
    LaserDartA1,
    LaserDartA2,
    LaserDartA3,
    LaserDartA4,
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
    WallDecalA,
    WallDecalB,
    FleshDecalA,
    FleshDecalB,
    ErrorRender,

    // These are different texture, but I need them to have the same enum type
    HUD_ShotgunA = 0,
    HUD_ShotgunFA,
    HUD_ShotgunFB,
    HUD_ShotgunRA,
    HUD_ShotgunRB,
    HUD_ShotgunRC,
    HUD_FlaregunA,
    HUD_FlaregunFA,
    HUD_FlaregunFB,
    HUD_TrishotA,
    HUD_TrishotFA,
    HUD_TrishotFB,
    HUD_CrossbowA,
    HUD_CrossbowFA,
    HUD_CrossbowFB,
    HUD_CrossbowFC,
    HUD_CrossbowFD,
    HUD_CrossbowFE,
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

    PowerItemsBegin,
    PickupMegaHealth,
    PickupMegaArmor,
    PowerItemsEnd,

    PickupBullets,
    PickupShells,
    PickupEnergy,
    PickupRockets,

    WeaponPickableBegin,
    PickupShotgun,
    PickupTrishot,
    PickupCrossbow,
    WeaponPickableEnd,

    PickableEnd,

    ProjectilesBegin,

    ProjectileRocket,
    ProjectileLaserDart,

    ProjectilesEnd,

    EffectBegin,
    EffectStatic, // Does nothing, has no collisions
    EffectDyingPlayer,
    EffectExplosion,
    EffectSpawn,
    EffectEnd,

    InventoryBegin,

    WeaponFlaregun,
    WeaponShotgun,
    WeaponTrishot,
    WeaponCrossbow,

    InventoryEnd,

    Decal
};

enum class ScriptId
{
    NoAction,
    FireFlare,
    FirePellets,
    FireBullet,
    FireLaserDart
};

enum class AnimationStateId
{
    Idle,
    Run,
    Missile,
    Recovery,
    MarkerLoop,
    MarkerDestroy,
    MarkerFreeze,
};

enum class AiState
{
    Start,
    WaitForRespawnRequest,
    RequestRespawn,
    WaitForRespawn
};
