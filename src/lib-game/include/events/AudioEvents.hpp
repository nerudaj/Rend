#pragma once

#include <core/Enums.hpp>
#include <core/Types.hpp>
#include <variant>

struct [[nodiscard]] ShotgunFiredAudioEvent
{
    PlayerStateIndexType channel;
    sf::Vector2f position;

    constexpr ShotgunFiredAudioEvent(
        PlayerStateIndexType channel,
        dgm::UniversalReference<sf::Vector2f> auto&& position) noexcept
        : channel(channel), position(std::forward<decltype(position)>(position))
    {
    }
};

struct BulletFiredAudioEvent
{
    PlayerStateIndexType channel;
    sf::Vector2f position;

    [[nodiscard]] constexpr BulletFiredAudioEvent(
        PlayerStateIndexType channel,
        dgm::UniversalReference<sf::Vector2f> auto&& position) noexcept
        : channel(channel), position(std::forward<decltype(position)>(position))
    {
    }
};

struct RocketFiredAudioEvent
{
    PlayerStateIndexType channel;
    sf::Vector2f position;

    [[nodiscard]] constexpr RocketFiredAudioEvent(
        PlayerStateIndexType channel,
        dgm::UniversalReference<sf::Vector2f> auto&& position) noexcept
        : channel(channel), position(std::forward<decltype(position)>(position))
    {
    }
};

struct [[nodiscard]] LaserCrossbowAudioEvent
{
    PlayerStateIndexType channel;
    sf::Vector2f position;

    constexpr LaserCrossbowAudioEvent(
        PlayerStateIndexType channel,
        dgm::UniversalReference<sf::Vector2f> auto&& position) noexcept
        : channel(channel), position(std::forward<decltype(position)>(position))
    {
    }
};

struct [[nodiscard]] LaserDartBouncedAudioEvent
{
    sf::Vector2f position;

    constexpr LaserDartBouncedAudioEvent(
        dgm::UniversalReference<sf::Vector2f> auto&& position) noexcept
        : position(std::forward<decltype(position)>(position))
    {
    }
};

struct [[nodiscard]] ExplosionTriggeredAudioEvent
{
    EntityType type;
    sf::Vector2f position;

    constexpr ExplosionTriggeredAudioEvent(
        EntityType type,
        dgm::UniversalReference<sf::Vector2f> auto&& position) noexcept
        : type(type), position(std::forward<decltype(position)>(position))
    {
    }
};

struct [[nodiscard]] PickablePickedUpAudioEvent
{
    EntityType type;
    PlayerStateIndexType channel;

    constexpr PickablePickedUpAudioEvent(
        EntityType type, PlayerStateIndexType channel) noexcept
        : type(type), channel(channel)
    {
    }
};

using AudioEvent = std::variant<
    ShotgunFiredAudioEvent,
    BulletFiredAudioEvent,
    RocketFiredAudioEvent,
    LaserCrossbowAudioEvent,
    LaserDartBouncedAudioEvent,
    ExplosionTriggeredAudioEvent,
    PickablePickedUpAudioEvent>;
