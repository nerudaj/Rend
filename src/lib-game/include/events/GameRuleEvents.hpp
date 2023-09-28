#pragma once

#include <DGM/dgm.hpp>
#include <core/Enums.hpp>
#include <variant>

struct PickablePickedUpGameEvent
{
    unsigned id;

    PickablePickedUpGameEvent(unsigned id) : id(id) {}
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

using GameEvent = std::variant<
    PickablePickedUpGameEvent,
    ProjectileCreatedGameEvent,
    ProjectileDestroyedGameEvent,
    EntityDestroyedGameEvent>;
