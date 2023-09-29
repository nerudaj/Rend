#pragma once

#include <DGM/dgm.hpp>
#include <core/Enums.hpp>
#include <variant>

struct PickablePickedUpGameEvent
{
    std::size_t entityIndex;

    PickablePickedUpGameEvent(std::size_t entityIndex)
        : entityIndex(entityIndex)
    {
    }
};

struct ProjectileCreatedGameEvent
{
    EntityType type;
    sf::Vector2f position;
    sf::Vector2f direction;

    ProjectileCreatedGameEvent(
        EntityType type,
        const sf::Vector2f& position,
        const sf::Vector2f& direction)
        : type(type), position(position), direction(direction)
    {
    }
};

struct ProjectileDestroyedGameEvent
{
    std::size_t entityIndex;

    ProjectileDestroyedGameEvent(std::size_t entityIndex)
        : entityIndex(entityIndex)
    {
    }
};

struct EntityDestroyedGameEvent
{
    std::size_t entityIndex;

    EntityDestroyedGameEvent(std::size_t entityIndex) : entityIndex(entityIndex)
    {
    }
};

struct PlayerRespawnedGameEvent
{
    std::size_t entityIndex;

    PlayerRespawnedGameEvent(std::size_t entityIndex) : entityIndex(entityIndex)
    {
    }
};

struct EffectSpawnedGameEvent
{
    EntityType type;
    sf::Vector2f position;

    EffectSpawnedGameEvent(EntityType type, const sf::Vector2f& position)
        : type(type), position(position)
    {
    }
};

struct PickupSpawnedGameEvent
{
    EntityType type;
    sf::Vector2f position;

    PickupSpawnedGameEvent(EntityType type, const sf::Vector2f& position)
        : type(type), position(position)
    {
    }
};

using GameEvent = std::variant<
    PickablePickedUpGameEvent,
    ProjectileCreatedGameEvent,
    ProjectileDestroyedGameEvent,
    EntityDestroyedGameEvent,
    PlayerRespawnedGameEvent,
    EffectSpawnedGameEvent,
    PickupSpawnedGameEvent>;
