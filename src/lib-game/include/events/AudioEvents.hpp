#pragma once

#include <core/Enums.hpp>
#include <core/Types.hpp>
#include <variant>

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

struct [[nodiscard]] SoundTriggeredAudioEvent
{
    const std::string sound;
    SoundSourceType sourceType;
    PlayerStateIndexType stateIdx;
    sf::Vector2f position;

    constexpr SoundTriggeredAudioEvent(
        const std::string& sound,
        SoundSourceType sourceType,
        PlayerStateIndexType stateIdx,
        const sf::Vector2f& position) noexcept
        : sound(sound)
        , sourceType(sourceType)
        , stateIdx(stateIdx)
        , position(position)
    {
    }
};

using AudioEvent = std::variant<
    LaserDartBouncedAudioEvent,
    PickablePickedUpAudioEvent,
    SoundTriggeredAudioEvent>;
