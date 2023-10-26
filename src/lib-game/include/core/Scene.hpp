#pragma once

#include <DGM/dgm.hpp>
#include <Settings.hpp>
#include <array>
#include <bitset>
#include <core/Constants.hpp>
#include <core/Enums.hpp>
#include <core/Types.hpp>
#include <input/AiController.hpp>
#include <input/SimpleController.hpp>
#include <utils/SemanticTypes.hpp>

import Memory;

struct HitscanResult
{
    sf::Vector2f impactPosition;
    std::optional<EntityIndexType> impactedEntityIdx = std::nullopt;
};

struct AnimationFrame
{
    SpriteId spriteId = SpriteId::ErrorRender;
    std::size_t duration = 10; // in ticks
    ScriptId scriptToTrigger = ScriptId::NoAction;
};

using AnimationClip = std::vector<AnimationFrame>;

struct AnimationState
{
    AnimationClip clip;
    AnimationStateId transition;
};

using EntityStates = std::map<AnimationStateId, AnimationState>;

struct EntityProperties
{
    float radius;
    SpriteId initialSpriteIndex;
    EntityStates states;
};

struct AnimationContext
{
    SpriteId spriteClipIndex;
    AnimationStateId animationStateId = AnimationStateId::Idle;
    std::size_t animationFrameIndex = 0;
    std::size_t
        lastAnimationUpdate; // number of ticks since last animation update
};

struct Entity
{
    EntityType typeId;
    AnimationContext animationContext;
    dgm::Circle hitbox;
    sf::Vector2f direction = NULL_VECTOR;

    // player stuff
    PlayerStateIndexType stateId = 0;
    int health = 0;
    int armor = 0;
};

struct MarkerDeadPlayer
{
    bool rebindCamera = false;
    PlayerStateIndexType stateId = 0;
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
    EntityIndexType ownerIdx;
    EntityType activeWeaponType;
    AnimationContext animationContext;
    int bulletCount = 0;
    int shellCount = 0;
    int energyCount = 0;
    int rocketCount = 0;
    AcquitedWeaponsArray acquiredWeapons;
};

struct AiBlackboard
{
    AiTopState aiTopState = AiTopState::Alive;
    AiState aiState = AiState::Start;
    mem::Rc<AiController> input;
    PlayerStateIndexType playerStateIdx;
    EntityIndexType trackedEnemyIdx = 0;
    sf::Vector2f targetLocation;
    sf::Vector2f nextStop;
    float seekTimeout = 0.f;
};

struct PlayerState
{
    SimpleController input;
    PlayerInventory inventory;
    std::optional<AiBlackboard> blackboard;
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

    std::size_t tick = 0;
    dgm::DynamicBuffer<Entity> things;
    dgm::DynamicBuffer<Marker> markers;
    Level level;
    dgm::SpatialIndex<EntityIndexType> spatialIndex;
    std::vector<PlayerState> playerStates;
    std::vector<sf::Vector2f> spawns;
    std::string mapname;
    EntityIndexType playerId = 0;
};

// TODO: move somewhere else
[[nodiscard]] static inline sf::Vector2f
getPerpendicular(const sf::Vector2f& v) noexcept
{
    return { -v.y, v.x };
}

static inline void forEachDirection(
    const sf::Vector2f& direction,
    const sf::Vector2f& plane,
    unsigned DIRECTION_COUNT,
    std::function<void(const sf::Vector2f&)> callback)
{
    for (unsigned i = 0; i < DIRECTION_COUNT; i++)
    {
        float x = 2.f * i / DIRECTION_COUNT - 1.f;
        callback(direction + plane * x);
    }
}

template<std::size_t Bits>
[[nodiscard]] static constexpr std::size_t
getPrevToggledBit(std::size_t index, const std::bitset<Bits>& bitset) noexcept
{
    assert(bitset[0]);
    if (index == 0) index = bitset.size();
    do
    {
        --index;
    } while (!bitset[index]);
    return index;
}

template<std::size_t Bits>
[[nodiscard]] static constexpr std::size_t
getNextToggledBit(std::size_t index, const std::bitset<Bits>& bitset) noexcept
{
    assert(bitset[0]);
    do
    {
        ++index;
    } while (index < bitset.size() && !bitset[index]);
    return index == bitset.size() ? 0 : index;
}

// Returns number (either -1.f, 0.f or 1.f) based on which
// direction to rotate 'from' vector to get to 'to'
[[nodiscard]] constexpr float
getVectorPivotDirection(const sf::Vector2f& from, const sf::Vector2f& to)
{
    const float m = -to.y * from.x + to.x * from.y;
    return m == 0.f ? 0.f : -m / std::abs(m);
}