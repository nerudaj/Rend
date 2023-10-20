#pragma once

#include <DGM/DGM.hpp>
#pragma warning(push, 0)
#include <SFML/Audio.hpp>
#pragma warning(pop)
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
