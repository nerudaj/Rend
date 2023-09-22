#pragma once

#include <DGM/dgm.hpp>
#include <Settings.hpp>
#include <core/Constants.hpp>
#include <input/ControllerInterface.hpp>

import Memory;

/// <summary>
/// Base class for all game objects
/// </summary>
struct BaseObject
{
    dgm::Circle body;
    unsigned currentSpriteId = 0;
    bool directionalSprite = false;
    sf::Vector2f direction;

    BaseObject(
        dgm::UniversalReference<dgm::Circle> auto&& body,
        SpriteId initialSpriteId,
        bool directionalSprite = false,
        const sf::Vector2f& direction = { 0.f, 0.f })
        : body(std::forward<decltype(body)>(body))
        , currentSpriteId(static_cast<unsigned>(initialSpriteId))
        , directionalSprite(directionalSprite)
        , direction(direction)
    {
    }
};

/// <summary>
/// Physical blockers in the world
/// </summary>
struct CollidableObject : public BaseObject
{
    CollidableObject(
        dgm::UniversalReference<dgm::Circle> auto&& body,
        SpriteId initialSpriteId,
        bool directionalSprite = false,
        const sf::Vector2f& direction = { 0.f, 0.f })
        : BaseObject(
            std::forward<decltype(body)>(body),
            initialSpriteId,
            directionalSprite,
            direction)
    {
    }
};

/// <summary>
/// Medkit, ammo, etc
/// </summary>
struct GenericPickup : public CollidableObject
{
    PickupId id;

    GenericPickup(
        dgm::UniversalReference<dgm::Circle> auto&& body,
        SpriteId initialSpriteId,
        PickupId id)
        : CollidableObject(std::forward<decltype(body)>(body), initialSpriteId)
        , id(id)
    {
    }
};

/// <summary>
/// Weapons on the floor
/// </summary>
struct WeaponPickup : public CollidableObject
{
    WeaponId id;

    WeaponPickup(
        dgm::UniversalReference<dgm::Circle> auto&& body,
        SpriteId initialSpriteId,
        WeaponId id)
        : CollidableObject(std::forward<decltype(body)>(body), initialSpriteId)
        , id(id)
    {
    }
};

struct Player : public CollidableObject
{
    mem::Rc<ControllerInterface> input;
    int health = 0;
    int armor = 0;
    int bulletCount = 0;
    int shellCount = 0;
    int energyCount = 0;
    int rocketCount = 0;

    Player(
        dgm::UniversalReference<dgm::Circle> auto&& body,
        dgm::UniversalReference<sf::Vector2f> auto&& dir,
        mem::Rc<ControllerInterface> in,
        SpriteId initialSpriteId)
        : CollidableObject(
            std::forward<decltype(body)>(body),
            initialSpriteId,
            true,
            std::forward<decltype(dir)>(dir))
        , input(in)
    {
    }
};

struct Projectile : CollidableObject
{
    sf::Vector2f forward;
    float damage;

    Projectile(
        dgm::UniversalReference<dgm::Circle> auto&& body,
        dgm::UniversalReference<sf::Vector2f> auto&& dir,
        SpriteId initialSpriteId,
        float damage)
        : CollidableObject(
            std::forward<decltype(body)>(body),
            initialSpriteId,
            true,
            std::forward<decltype(dir)>(dir))
        , forward({0.f, 0.f})
        , damage(damage)
    {
    }
};

using GameObject = std::variant<
    BaseObject,
    Player,
    CollidableObject,
    GenericPickup,
    WeaponPickup,
    Projectile>;

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

    // Player player;
    Level level;
    dgm::SpatialBuffer<GameObject> things;
    std::vector<sf::Vector2f> spawns;
    std::string mapname;
    unsigned playerId = 0;

    static Scene buildScene(
        const dgm::ResourceManager& resmgr,
        const sf::Vector2f& baseResolution,
        const Settings& settings);
};

// TODO: move somewhere else
template<class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};

// TODO: move somewhere else
static inline float operator*(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}

// TODO: move somewhere else
static inline sf::Vector2f
getPerpendicular(const sf::Vector2f& v) noexcept
{
    return { -v.y, v.x };
}

struct Entity
{
    EntityType typeId;
    SpriteId spriteClipIndex;
    dgm::Circle hitbox;
    sf::Vector2f direction;

    // player stuff
    short inputId;
    short health;
    short armor;
    short bulletCount;
    short shellCount;
    short rocketCount;
    short energyCount;
};