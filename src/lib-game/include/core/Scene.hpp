#pragma once

#include <DGM/dgm.hpp>
#include <Settings.hpp>
#include <array>
#include <core/Constants.hpp>
#include <core/Enums.hpp>
#include <input/SimpleController.hpp>
#include <utils/SemanticTypes.hpp>

import Memory;

using AnimationClip = std::vector<SpriteId>;

struct AnimationState
{
    AnimationClip clip;
    std::size_t updateFrequency; // how many frames pass before next update
    AnimationStateId transition;
};

using EntityStates = std::map<AnimationStateId, AnimationState>;

struct EntityProperties
{
    float radius;
    SpriteId initialSpriteIndex;
    EntityStates states;
};

struct RenderState
{
    SpriteId spriteClipIndex;
    AnimationStateId animationStateId = AnimationStateId::Idle;
    std::size_t animationFrameIndex = 0;
    std::size_t
        lastAnimationUpdate; // number of frames since last animation update
};

struct Entity
{
    EntityType typeId;
    RenderState renderState;
    dgm::Circle hitbox;
    sf::Vector2f direction = NULL_VECTOR;

    // player stuff
    int stateId = 0;
    int health = 0;
    int armor = 0;
};

struct MarkerDeadPlayer
{
    bool rebindCamera = false;
    int stateId = -1;
};

struct MarkerItemRespawner
{
    bool spawnEffectEmitted = false;
    float timeout = 0.f;
    EntityType pickupType;
    sf::Vector2f position;
};

struct PlayerInventory
{
    EntityType activeWeaponType;
    RenderState renderState;
    int bulletCount = 0;
    int shellCount = 0;
    int energyCount = 0;
    int rocketCount = 0;
    float fireCooldown = 0.f;
};

struct PlayerState
{
    SimpleController input;
    PlayerInventory inventory;
};

using Marker = std::variant<MarkerDeadPlayer, MarkerItemRespawner>;

struct Level
{
    unsigned width;
    unsigned heightHint;
    dgm::Mesh bottomMesh;
    dgm::Mesh upperMesh;
};

// Add all game actors and objects to this struct as it is passed
// between all engines and automatically created in AppStateIngame
// using buildScene function
// After adding actors here, you might want to update render/RenderContext.hpp
// with objects required to render your actors
struct Scene
{
    dgm::Camera worldCamera;
    dgm::Camera hudCamera;

    std::size_t frameId = 0;
    dgm::DynamicBuffer<Entity> things;
    dgm::DynamicBuffer<Marker> markers;
    Level level;
    dgm::SpatialBuffer<Entity> spatialIndex;
    std::array<PlayerState, MAX_PLAYER_COUNT> playerStates;
    std::vector<sf::Vector2f> spawns;
    std::string mapname;
    std::size_t playerId = 0;
};

// TODO: move somewhere else
static inline sf::Vector2f getPerpendicular(const sf::Vector2f& v) noexcept
{
    return { -v.y, v.x };
}
