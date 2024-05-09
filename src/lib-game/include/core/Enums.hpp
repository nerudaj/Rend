#pragma once

#include <LevelItemId.hpp>
#include <cstdint>
#include <map>
#include <string>

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
    LauncherA,
    BallistaA,
    HarpoonA,
    PillarA,
    FloorLampA,
    CeilLampA,
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
    PlayerFA0,
    PlayerFA1,
    PlayerFA2,
    PlayerFA3,
    PlayerFA4,
    PlayerFB0,
    PlayerFB1,
    PlayerFB2,
    PlayerFB3,
    PlayerFB4,
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
    FlareA0,
    FlareA1,
    FlareA2,
    FlareA3,
    FlareA4,
    ExplosionA,
    ExplosionB,
    ExplosionC,
    ExplosionD,
    ExplosionE,
    ExplosionF,
    DartExplosionA,
    DartExplosionB,
    DartExplosionC,
    FlareExplosionA,
    FlareExplosionB,
    FlareExplosionC,
    FlareExplosionD,
    FlareExplosionE,
    FlareExplosionF,
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
    RailDecalA,
    RailDecalB,
    RailDecalC,
    ErrorRender,

    // These are different texture, but I need them to have the same enum
    // type
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
    HUD_TrishotFC,
    HUD_TrishotFD,
    HUD_CrossbowA,
    HUD_CrossbowFA,
    HUD_CrossbowFB,
    HUD_CrossbowFC,
    HUD_CrossbowFD,
    HUD_CrossbowFE,
    HUD_LauncherA,
    HUD_LauncherFA,
    HUD_LauncherFB,
    HUD_LauncherFC,
    HUD_LauncherFD,
    HUD_BallistaA,
    HUD_BallistaB,
    HUD_BallistaC,
    HUD_BallistaFA,
    HUD_BallistaFB,
    HUD_BallistaFC,
    HUD_BallistaFD,
    HUD_BallistaFE,
    HUD_BallistaFF,
    HUD_BallistaFG,
    HUD_BallistaRA,
    HUD_BallistaRB,
    HUD_BallistaRC,
    HUD_BallistaRD,
    HUD_BallistaRE,

    // Again, different texture
    HUD_Health = 0,
    HUD_Armor,
    HUD_BulletAmmo,
    HUD_ShellAmmo,
    HUD_EnergyAmmo,
    HUD_RocketAmmo,
    HUD_FlaregunOutline,
    HUD_ShotgunOutline,
    HUD_TrishotOutline,
    HUD_CrossbowOutline,
    HUD_LauncherOutline,
    HUD_BallistaOutline,
    HUD_FlaregunFilled,
    HUD_ShotgunFilled,
    HUD_TrishotFilled,
    HUD_CrossbowFilled,
    HUD_LauncherFilled,
    HUD_BallistaFilled,
    HUD_SelectFrame,
    HUD_End,
};

enum class EntityType
{
    Error,
    None,
    CameraAnchor,
    StaticDecorationBegin,
    Pillar,
    FloorLamp,
    CeilLamp,
    StaticDecorationEnd,
    Player,

    PickableBegin,

    PickableHealthArmorBegin,
    PickupHealth,
    PickupArmor,

    PowerItemsBegin,
    PickupMegaHealth,
    PickupMegaArmor,
    PowerItemsEnd,
    PickableHealthArmorEnd,

    PickupBullets,
    PickupShells,
    PickupEnergy,
    PickupRockets,

    WeaponPickableBegin,
    PickupShotgun,
    PickupTrishot,
    PickupCrossbow,
    PickupLauncher,
    PickupBallista,
    WeaponPickableEnd,

    PickableEnd,

    ProjectilesBegin,

    ProjectileFlare,
    ProjectileRocket,
    ProjectileLaserDart,

    ProjectilesEnd,

    EffectBegin,
    EffectStatic, // Does nothing, has no collisions
    EffectDyingPlayer,
    EffectFlareExplosion,
    EffectRocketExplosion,
    EffectDartExplosion,
    EffectSpawn,
    EffectRailDecal,
    EffectEnd,

    InventoryBegin,

    WeaponFlaregun,
    WeaponShotgun,
    WeaponTrishot,
    WeaponCrossbow,
    WeaponLauncher,
    WeaponBallista,
    WeaponHarpoon,

    InventoryEnd,

    Decal
};

enum class Trait
{
    None = 0,
    Solid = 0b1,
    Destructible = 0b10,
    Directional = 0b100,
    Projectile = 0b1000,
    Bouncy = 0b10000,
    Explosive = 0b100000,
    Pickable = 0b1000000,
    PowerItem = 0b10000000,
    WeaponPickup = 0b100000000,
};

constexpr Trait operator|(Trait a, Trait b)
{
    using T = std::underlying_type_t<Trait>;
    return static_cast<Trait>(static_cast<T>(a) | static_cast<T>(b));
}

constexpr bool operator&(Trait base, Trait question)
{
    using T = std::underlying_type_t<Trait>;
    return static_cast<T>(base) & static_cast<T>(question);
}

enum class AmmoType
{
    Bullets = 0,
    Shells = 1,
    Energy = 2,
    Rockets = 3
};

enum class ScriptId
{
    NoAction,
    FirePellets,
    FireBullet,
    FireRay,
    FireProjectile,
    ReleaseTrigger,
    TriggerSound,
};

enum class SoundSourceType
{
    Ambient,
    Pov,
    Player,
};

enum class AnimationStateId
{
    Idle,
    Run,
    Missile,
    Recovery,
    Lower,
    Raise,
    FastLower,
    FastRaise,
    MarkerLoop,
    MarkerDestroy,
    MarkerFreeze, // Candidate for deletion
};
