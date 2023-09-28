#pragma once

#include <DGM/dgm.hpp>
#include <Settings.hpp>
#include <core/Constants.hpp>
#include <input/SimpleController.hpp>

import Memory;

struct Entity
{
    EntityType typeId;
    SpriteId spriteClipIndex;
    dgm::Circle hitbox;
    sf::Vector2f direction = NULL_VECTOR;

    // player stuff
    int inputId = 0;
    int health = 0;
    int armor = 0;
    int bulletCount = 0;
    int shellCount = 0;
    int energyCount = 0;
    int rocketCount = 0;
    float fireCooldown = 0.f;

    AnimationId animationStateId;
    std::size_t animationFrameIndex;
    std::size_t
        lastAnimationUpdate; // number of frames since last animation update
};

struct Level
{
    unsigned width;
    unsigned heightHint;
    dgm::Mesh bottomMesh;
    dgm::Mesh upperMesh;
};

struct Position
{
    sf::Vector2f value;
};

struct Direction
{
    sf::Vector2f value;
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
    Level level;
    dgm::SpatialBuffer<Entity> spatialIndex;
    std::array<SimpleController, MAX_PLAYER_COUNT> inputs;
    std::vector<sf::Vector2f> spawns;
    std::string mapname;
    std::size_t playerId = 0;

    [[nodiscard]] static Scene buildScene(
        const dgm::ResourceManager& resmgr,
        const sf::Vector2f& baseResolution,
        const Settings& settings);

    [[nodiscard]] static Entity createPlayer(
        const Position& position,
        const Direction& lookDirection,
        short inputId);

    [[nodiscard]] static Entity createProjectile(
        EntityType type, const Position& position, const Direction& direction);

    [[nodiscard]] static Entity
    createEffect(SpriteId spriteClipIndex, const Position& position);
};

// TODO: move somewhere else
static inline sf::Vector2f getPerpendicular(const sf::Vector2f& v) noexcept
{
    return { -v.y, v.x };
}
