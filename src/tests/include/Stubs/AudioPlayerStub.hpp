#pragma once

import AudioPlayer;

#include <SFML/System/Vector2.hpp>
#include <optional>
#include <string>

class AudioPlayerStub : public AudioPlayerInterface
{
public:
    void playSoundOnChannel(
        const std::string&,
        const std::size_t,
        const bool,
        std::optional<sf::Vector2f>) override
    {
    }
};
