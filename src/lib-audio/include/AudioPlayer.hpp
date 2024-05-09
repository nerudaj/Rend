#pragma once

#pragma warning(push, 0)
#include <DGM/DGM.hpp>
#include <SFML/Audio.hpp>
#pragma warning(pop)
#include "AudioPlayerInterface.hpp"
#include <Memory.hpp>
#include <vector>

constexpr const unsigned CHANNEL_COUNT = 10;

class [[nodiscard]] AudioPlayer final : public AudioPlayerInterface
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
        std::optional<sf::Vector2f> position = std::nullopt) override;

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
