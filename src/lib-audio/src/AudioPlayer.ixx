module;

#include <DGM/DGM.hpp>
#pragma warning(push, 0)
#include <SFML/Audio.hpp>
#pragma warning(pop)
#include <vector>

export module AudioPlayer;

import Memory;

export constexpr const unsigned CHANNEL_COUNT = 10;

export class [[nodiscard]] AudioPlayer
{
public:
    AudioPlayer(
        const std::size_t channelCount,
        mem::Rc<const dgm::ResourceManager> resmgr) noexcept
        : channels(channelCount), resmgr(resmgr)
    {
    }

    ~AudioPlayer();

public:
    void playSoundOnChannel(
        const std::string& soundName,
        const std::size_t channel,
        const bool force = false,
        std::optional<sf::Vector2f> position = std::nullopt);

    void stopChannel(const std::size_t channel);

    void setSoundVolume(const float volume);

private:
    std::vector<sf::Sound> channels;
    mem::Rc<const dgm::ResourceManager> resmgr;

    [[nodiscard]] bool isChannelActive(const std::size_t channel) const
    {
        return channels[channel].getStatus() == sf::Sound::Status::Playing;
    }
};
