#pragma once

#include <DGM/DGM.hpp>
#include <SFML/Audio.hpp>
#include <vector>

import Memory;

constexpr unsigned CHANNEL_COUNT = 8;

class AudioPlayer
{
private:
    std::vector<sf::Sound> channels;
    mem::Rc<const dgm::ResourceManager> resmgr;

    [[nodiscard]] bool isChannelActive(const unsigned channel) const
    {
        return channels[channel].getStatus() == sf::Sound::Status::Playing;
    }

public:
    void playSoundOnChannel(
        const std::string& soundName,
        const unsigned channel,
        const bool force = false);

    void stopChannel(const unsigned channel);

    void setSoundVolume(const float volume);

    [[nodiscard]] AudioPlayer(
        const unsigned channelCount,
        mem::Rc<const dgm::ResourceManager> resmgr) noexcept
        : channels(channelCount), resmgr(resmgr)
    {
    }

    ~AudioPlayer();
};
