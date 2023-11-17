#pragma once

#include <LevelItemId.hpp>
#include <cstdint>
#include <map>
#include <string>

enum class TilesetMapping
{
    Floor1 = 0,
    Floor2,
    CeilSky = 2,
    CeilLow,
    // TODO: CeilHigh
    Crate1 = 4,
    Crate2,
    Wall1 = 6,
    End = 16
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
    LauncherA,
    BallistaA,
    HarpoonA,
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
};

enum class EntityType
{
    Error,
    None,
    StaticDecorationBegin,
    Pillar,
    FloorLamp,
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
    FireFlare,
    FirePellets,
    FireBullet,
    FireLaserDart,
    FireRocket,
    FireHarpoon,
    ReleaseTrigger,
    PlayRecoverySound,
};

enum class AnimationStateId
{
    Idle,
    Run,
    Missile,
    Recovery,
    MarkerLoop,
    MarkerDestroy,
    MarkerFreeze, // Candidate for deletion
};

enum class AiTopState
{
    BootstrapAlive,
    Alive,
    BootstrapDead,
    Dead
};

const std::map<AiTopState, std::string> TOP_STATES_TO_STRING = {
    { AiTopState::BootstrapAlive, "BootstrapAlive" },
    { AiTopState::Alive, "Alive" },
    { AiTopState::BootstrapDead, "BootstrapDead" },
    { AiTopState::Dead, "Dead" }
};

enum class AiState
{
    Start,

    PickNextJumpPoint,
    TryToPickNewTarget,
    ShootTarget,
    SwapWeapon,
    Delay,

    Update,

    WaitForRespawnRequest,
    RequestRespawn,
    WaitForRespawn
};

const std::map<AiState, std::string> AI_STATE_TO_STRING = {
    { AiState::Start, "Start" },
    { AiState::PickNextJumpPoint, "PickNextJumpPoint" },
    { AiState::TryToPickNewTarget, "TryToPickNewTarget" },
    { AiState::ShootTarget, "ShootTarget" },
    { AiState::SwapWeapon, "SwapWeapon" },
    { AiState::Delay, "Delay" },
    { AiState::Update, "Update" },
    { AiState::WaitForRespawnRequest, "WaitForRespawnRequest" },
    { AiState::RequestRespawn, "RequestRespawn" },
    { AiState::WaitForRespawn, "WaitForRespawn" },
};
