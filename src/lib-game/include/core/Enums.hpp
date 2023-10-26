#pragma once

#include <cstdint>
#include <map>
#include <string>

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
    Launcher,
    Ballista,
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
    LauncherA,
    BallistaA,
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
    DartExplosionA,
    DartExplosionB,
    DartExplosionC,
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
    HUD_LauncherA,
    HUD_LauncherFA,
    HUD_LauncherFB,
    HUD_LauncherFC,
    HUD_LauncherFD,
    HUD_BallistaA,
    HUD_BallistaFA,
    HUD_BallistaFB,
    HUD_BallistaFC,
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
    PickupLauncher,
    PickupBallista,
    WeaponPickableEnd,

    PickableEnd,

    ProjectilesBegin,

    ProjectileRocket,
    ProjectileLaserDart,

    ProjectilesEnd,

    EffectBegin,
    EffectStatic, // Does nothing, has no collisions
    EffectDyingPlayer,
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

    InventoryEnd,

    Decal
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

enum class AiTopState
{
    BootstrapAlive,
    Alive,
    BootstrapDead,
    Dead
};

const std::map<AiTopState, std::string> TOP_STATES_TO_STRING = {
    { AiTopState::BootstrapAlive, "BootstrapAlive" },
    { AiTopState::BootstrapAlive, "Alive" },
    { AiTopState::BootstrapAlive, "BootstrapAlive" },
    { AiTopState::BootstrapAlive, "Dead" }
};

enum class AiState
{
    Start,

    PickNextJumpPoint,
    TryToPickNewTarget,
    ShootTarget,

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
    { AiState::Update, "Update" },
    { AiState::WaitForRespawnRequest, "WaitForRespawnRequest" },
    { AiState::RequestRespawn, "RequestRespawn" },
    { AiState::WaitForRespawn, "WaitForRespawn" },
};
