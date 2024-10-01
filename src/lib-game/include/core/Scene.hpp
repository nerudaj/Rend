#pragma once

#include "utils/HudMessage.hpp"
#include <DGM/dgm.hpp>
#include <Memory.hpp>
#include <ai/AiEnums.hpp>
#include <array>
#include <bitset>
#include <core/Constants.hpp>
#include <core/Enums.hpp>
#include <core/Types.hpp>
#include <enums/SkyboxTheme.hpp>
#include <enums/SpawnRotation.hpp>
#include <enums/TexturePack.hpp>
#include <input/AiController.hpp>
#include <input/SimpleController.hpp>
#include <render/DrawableLevel.hpp>
#include <utils/DistanceIndex.hpp>
#include <utils/SemanticTypes.hpp>

struct [[nodiscard]] HitscanResult final
{
    sf::Vector2f impactPosition;
    std::optional<EntityIndexType> impactedEntityIdx = std::nullopt;
};

struct [[nodiscard]] AnimationContext final
{
    SpriteId spriteClipIndex;
    AnimationStateId animationStateId = AnimationStateId::Idle;
    std::size_t animationFrameIndex = 0;
    std::size_t
        lastAnimationUpdate; // number of ticks since last animation update
};

struct [[nodiscard]] Entity final
{
    EntityType typeId;
    AnimationContext animationContext;
    dgm::Circle hitbox;
    sf::Vector2f direction = NULL_VECTOR;

    // player stuff
    PlayerStateIndexType stateIdx = 0;
    int health = 0;
    int armor = 0;
};

struct MarkerDeadPlayer
{
    bool rebindCamera = false;
    PlayerStateIndexType stateIdx = 0;
};

struct MarkerItemRespawner
{
    bool spawnEffectEmitted = false;
    float timeout = 0.f;
    EntityType pickupType;
    sf::Vector2f position;
};

using Marker = std::variant<MarkerDeadPlayer, MarkerItemRespawner>;

struct PlayerInventory
{
    EntityIndexType ownerIdx;
    EntityType activeWeaponType;
    EntityType lastWeaponType;
    AnimationContext animationContext;
    AmmoArray ammo = { 0, 0, 0, PLAYER_INITIAL_ROCKETS };
    AcquiredWeaponsArray acquiredWeapons;
    int score = 0;
    float selectionTimeout = 0.f;
    std::size_t selectionIdx = 0;
    Team team = Team::Red;
};

struct AiBlackboard
{
    mem::Rc<AiController> input;
    AiTopState aiTopState = AiTopState::BootstrapDead;
    AiState aiState = AiState::ChoosingGatherLocation;
    AiState delayedTransitionState;
    AiPersonality personality = AiPersonality::Default;
    PlayerStateIndexType playerStateIdx;
    EntityIndexType targetEnemyIdx = 0;
    WeaponIndexType targetWeaponIdx = 0;
    sf::Vector2f targetLocation;
    sf::Vector2f longTermTargetLocation;
    EntityType targetWeaponToSwapTo = EntityType::WeaponFlaregun;
    int lastHealth = 100;
    float targettingTimer;
};

struct RenderContext
{
    HudMessage message;
    float redOverlayIntensity = 0.f;
};

struct PlayerState
{
    SimpleController input;
    PlayerInventory inventory;
    RenderContext renderContext;
    std::optional<AiBlackboard> blackboard;
    bool autoswapOnPickup = false;
};

struct Level
{
    unsigned width = 0;
    unsigned height = 0;
    dgm::Mesh bottomMesh;
    dgm::Mesh upperMesh;
    SkyboxTheme skyboxTheme = SkyboxTheme {};
    TexturePack texturePack = TexturePack {};
};

struct CameraContext
{
    EntityIndexType anchorIdx = 0;
};

struct [[nodiscard]] Spawn final
{
    sf::Vector2f position;
    sf::Vector2f direction;
    Team team;
};

// Add all game actors and objects to this struct as it is passed
// between all engines and automatically created in AppStateIngame
// using buildScene function
// After adding actors here, you might want to update render/RenderContext.hpp
// with objects required to render your actors
struct Scene
{
    // This has to be backed up
    std::size_t tick = 0;
    dgm::DynamicBuffer<Entity> things = {};
    dgm::DynamicBuffer<Marker> markers = {};
    std::vector<PlayerState> playerStates = {};

    CameraContext camera;

    // This doesn't have to be backed up
    Level level;
    DrawableLevel drawableLevel;
    dgm::SpatialIndex<EntityIndexType> spatialIndex;
    DistanceIndex distanceIndex;
    std::vector<Spawn> spawns = {};
    std::vector<sf::Vector2f> greyFlagSpawns = {};
    std::vector<sf::Vector2f> dummyAiDestinations = {};
    dgm::WorldNavMesh navmesh;
};
