#pragma once

#include <DGM/dgm.hpp>
#include <core/EntityDefinitionTypes.hpp>
#include <core/Enums.hpp>
#include <variant>

struct PickablePickedUpGameEvent
{
    EntityIndexType entityIndex;

    [[nodiscard]] constexpr PickablePickedUpGameEvent(
        EntityIndexType entityIndex) noexcept
        : entityIndex(entityIndex)
    {
    }
};

struct ProjectileCreatedGameEvent
{
    EntityType type;
    sf::Vector2f position;
    sf::Vector2f direction;
    PlayerStateIndexType originatorStateIdx;

    [[nodiscard]] ProjectileCreatedGameEvent(
        EntityType type,
        const sf::Vector2f& position,
        const sf::Vector2f& direction,
        PlayerStateIndexType originatorStateIdx) noexcept
        : type(type)
        , position(position)
        , direction(direction)
        , originatorStateIdx(originatorStateIdx)
    {
    }
};

struct ProjectileDestroyedGameEvent
{
    EntityIndexType entityIndex;

    [[nodiscard]] constexpr ProjectileDestroyedGameEvent(
        EntityIndexType entityIndex) noexcept
        : entityIndex(entityIndex)
    {
    }
};

struct EntityDestroyedGameEvent
{
    EntityIndexType entityIndex;

    [[nodiscard]] constexpr EntityDestroyedGameEvent(
        EntityIndexType entityIndex) noexcept
        : entityIndex(entityIndex)
    {
    }
};

struct PlayerRespawnedGameEvent
{
    MarkerIndexType markerIndex;

    [[nodiscard]] constexpr PlayerRespawnedGameEvent(
        MarkerIndexType markerIndex) noexcept
        : markerIndex(markerIndex)
    {
    }
};

struct EffectSpawnedGameEvent
{
    EntityType type;
    sf::Vector2f position;

    [[nodiscard]] EffectSpawnedGameEvent(
        EntityType type, const sf::Vector2f& position) noexcept
        : type(type), position(position)
    {
    }
};

struct PickupSpawnedGameEvent
{
    EntityType type;
    sf::Vector2f position;
    std::size_t markerIndex;

    [[nodiscard]] PickupSpawnedGameEvent(
        EntityType type,
        const sf::Vector2f& position,
        std::size_t markerIndex) noexcept
        : type(type), position(position), markerIndex(markerIndex)
    {
    }
};

struct HitscanProjectileFiredGameEvent
{
    HitscanResult hit;
    int damage;
    PlayerStateIndexType originatorStateIdx;

    [[nodiscard]] HitscanProjectileFiredGameEvent(
        HitscanResult hit,
        int damage,
        PlayerStateIndexType originatorStateIdx) noexcept
        : hit(hit), damage(damage), originatorStateIdx(originatorStateIdx)
    {
    }
};

struct ScriptTriggeredGameEvent
{
    Script script;
    EntityIndexType targetEntityIdx;

    [[nodiscard]] constexpr ScriptTriggeredGameEvent(
        Script script, EntityIndexType targetEntityIdx) noexcept
        : script(script), targetEntityIdx(targetEntityIdx)
    {
    }
};

struct PlayerKilledThemselvesGameEvent
{
    PlayerStateIndexType playerStateIdx;

    [[nodiscard]] constexpr PlayerKilledThemselvesGameEvent(
        PlayerStateIndexType idx) noexcept
        : playerStateIdx(idx)
    {
    }
};

struct PlayerKilledPlayerGameEvent
{
    PlayerStateIndexType killerStateIdx;
    PlayerStateIndexType victimStateIdx;

    [[nodiscard]] constexpr PlayerKilledPlayerGameEvent(
        PlayerStateIndexType killerStateIdx,
        PlayerStateIndexType victimStateIdx) noexcept
        : killerStateIdx(killerStateIdx), victimStateIdx(victimStateIdx)
    {
    }
};

struct WeaponPickedUpGameEvent
{
    size_t weaponIdx;
    EntityIndexType playerIdx;

    [[nodiscard]] constexpr WeaponPickedUpGameEvent(
        size_t weaponIdx, EntityIndexType playerIdx) noexcept
        : weaponIdx(weaponIdx), playerIdx(playerIdx)
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
    PickupSpawnedGameEvent,
    HitscanProjectileFiredGameEvent,
    ScriptTriggeredGameEvent,
    PlayerKilledThemselvesGameEvent,
    PlayerKilledPlayerGameEvent,
    WeaponPickedUpGameEvent>;
