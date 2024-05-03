#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>
#include <optional>

class [[nodiscard]] AudioPlayerInterface
{
public:
    virtual ~AudioPlayerInterface() = default;

public:
    virtual void playSoundOnChannel(
        const std::string& soundName,
        const std::size_t channel,
        const bool force = false,
        std::optional<sf::Vector2f> position = std::nullopt) = 0;
};
