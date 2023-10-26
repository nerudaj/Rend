#pragma once

#include <core/Enums.hpp>
#include <core/Types.hpp>
#include <variant>

struct ShotgunFiredAudioEvent
{
    PlayerStateIndexType channel;

    [[nodiscard]] constexpr ShotgunFiredAudioEvent(
        PlayerStateIndexType channel) noexcept
        : channel(channel)
    {
    }
};

struct BulletFiredAudioEvent
{
    PlayerStateIndexType channel;

    [[nodiscard]] constexpr BulletFiredAudioEvent(
        PlayerStateIndexType channel) noexcept
        : channel(channel)
    {
    }
};

struct RocketFiredAudioEvent
{
    PlayerStateIndexType channel;

    [[nodiscard]] constexpr RocketFiredAudioEvent(
        PlayerStateIndexType channel) noexcept
        : channel(channel)
    {
    }
};

struct LaserCrossbowAudioEvent
{
    PlayerStateIndexType channel;

    [[nodiscard]] constexpr LaserCrossbowAudioEvent(
        PlayerStateIndexType channel) noexcept
        : channel(channel)
    {
    }
};

struct LaserDartBouncedAudioEvent
{
};

struct ExplosionTriggeredAudioEvent
{
    EntityType type;

    [[nodiscard]] constexpr ExplosionTriggeredAudioEvent(
        EntityType type) noexcept
        : type(type)
    {
    }
};

using AudioEvent = std::variant<
    ShotgunFiredAudioEvent,
    BulletFiredAudioEvent,
    RocketFiredAudioEvent,
    LaserCrossbowAudioEvent,
    LaserDartBouncedAudioEvent,
    ExplosionTriggeredAudioEvent>;
