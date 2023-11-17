#pragma once

#include <core/Enums.hpp>
#include <core/Types.hpp>
#include <variant>

struct [[nodiscard]] FlaregunFiredAudioEvent
{
    PlayerStateIndexType stateIdx;
    sf::Vector2f position;

    constexpr FlaregunFiredAudioEvent(
        PlayerStateIndexType stateIdx,
        dgm::UniversalReference<sf::Vector2f> auto&& position) noexcept
        : stateIdx(stateIdx)
        , position(std::forward<decltype(position)>(position))
    {
    }
};

struct [[nodiscard]] ShotgunFiredAudioEvent
{
    PlayerStateIndexType stateIdx;
    sf::Vector2f position;

    constexpr ShotgunFiredAudioEvent(
        PlayerStateIndexType stateIdx,
        dgm::UniversalReference<sf::Vector2f> auto&& position) noexcept
        : stateIdx(stateIdx)
        , position(std::forward<decltype(position)>(position))
    {
    }
};

struct BulletFiredAudioEvent
{
    PlayerStateIndexType stateIdx;
    sf::Vector2f position;

    [[nodiscard]] constexpr BulletFiredAudioEvent(
        PlayerStateIndexType stateIdx,
        dgm::UniversalReference<sf::Vector2f> auto&& position) noexcept
        : stateIdx(stateIdx)
        , position(std::forward<decltype(position)>(position))
    {
    }
};

struct RocketFiredAudioEvent
{
    PlayerStateIndexType stateIdx;
    sf::Vector2f position;

    [[nodiscard]] constexpr RocketFiredAudioEvent(
        PlayerStateIndexType stateIdx,
        dgm::UniversalReference<sf::Vector2f> auto&& position) noexcept
        : stateIdx(stateIdx)
        , position(std::forward<decltype(position)>(position))
    {
    }
};

struct [[nodiscard]] LaserCrossbowAudioEvent
{
    PlayerStateIndexType stateIdx;
    sf::Vector2f position;

    constexpr LaserCrossbowAudioEvent(
        PlayerStateIndexType stateIdx,
        dgm::UniversalReference<sf::Vector2f> auto&& position) noexcept
        : stateIdx(stateIdx)
        , position(std::forward<decltype(position)>(position))
    {
    }
};

struct [[nodiscard]] LaserDartBouncedAudioEvent
{
    PlayerStateIndexType stateIdx;
    sf::Vector2f position;

    constexpr LaserDartBouncedAudioEvent(
        PlayerStateIndexType stateIdx,
        dgm::UniversalReference<sf::Vector2f> auto&& position) noexcept
        : stateIdx(stateIdx)
        , position(std::forward<decltype(position)>(position))
    {
    }
};

struct [[nodiscard]] ExplosionTriggeredAudioEvent
{
    PlayerStateIndexType stateIdx;
    EntityType type;
    sf::Vector2f position;

    constexpr ExplosionTriggeredAudioEvent(
        EntityType type,
        PlayerStateIndexType stateIdx,
        dgm::UniversalReference<sf::Vector2f> auto&& position) noexcept
        : type(type)
        , stateIdx(stateIdx)
        , position(std::forward<decltype(position)>(position))
    {
    }
};

struct [[nodiscard]] PickablePickedUpAudioEvent
{
    EntityType type;
    PlayerStateIndexType stateIdx;

    constexpr PickablePickedUpAudioEvent(
        EntityType type, PlayerStateIndexType stateIdx) noexcept
        : type(type), stateIdx(stateIdx)
    {
    }
};

struct [[nodiscard]] WeaponRecoveringAudioEvent
{
    EntityType type;
    PlayerStateIndexType stateIdx;

    constexpr WeaponRecoveringAudioEvent(
        EntityType type, PlayerStateIndexType stateIdx) noexcept
        : type(type), stateIdx(stateIdx)
    {
    }
};

using AudioEvent = std::variant<
    FlaregunFiredAudioEvent,
    ShotgunFiredAudioEvent,
    BulletFiredAudioEvent,
    RocketFiredAudioEvent,
    LaserCrossbowAudioEvent,
    LaserDartBouncedAudioEvent,
    ExplosionTriggeredAudioEvent,
    PickablePickedUpAudioEvent,
    WeaponRecoveringAudioEvent>;
