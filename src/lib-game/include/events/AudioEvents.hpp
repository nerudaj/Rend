#pragma once

#include <core/Enums.hpp>
#include <core/Types.hpp>
#include <variant>

struct [[nodiscard]] SoundTriggeredAudioEvent
{
    const std::string sound;
    SoundSourceType sourceType;
    PlayerStateIndexType stateIdx;
    sf::Vector2f position = { 0.f, 0.f };

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

    constexpr SoundTriggeredAudioEvent(
        const std::string& sound,
        PlayerStateIndexType stateIdx,
        const sf::Vector2f& position = { 0.f, 0.f }) noexcept
        : sound(sound)
        , sourceType(SoundSourceType::Pov)
        , stateIdx(stateIdx)
        , position(position)
    {
    }
};

using AudioEvent = std::variant<SoundTriggeredAudioEvent>;
