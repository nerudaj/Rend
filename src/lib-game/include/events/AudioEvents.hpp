#pragma once

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

struct FlareFiredAudioEvent
{
    PlayerStateIndexType channel;

    [[nodiscard]] constexpr FlareFiredAudioEvent(
        PlayerStateIndexType channel) noexcept
        : channel(channel)
    {
    }
};

struct LaserDartAudioEvent
{
    PlayerStateIndexType channel;

    [[nodiscard]] constexpr LaserDartAudioEvent(
        PlayerStateIndexType channel) noexcept
        : channel(channel)
    {
    }
};

using AudioEvent = std::variant<
    ShotgunFiredAudioEvent,
    BulletFiredAudioEvent,
    FlareFiredAudioEvent,
    LaserDartAudioEvent>;
