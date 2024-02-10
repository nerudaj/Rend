module;

#pragma warning(push, 0)
#include <DGM/DGM.hpp>
#include <SFML/Audio.hpp>
#pragma warning(pop)
#include <vector>

export module AudioPlayer;

import Memory;

export constexpr const unsigned CHANNEL_COUNT = 10;

export class [[nodiscard]] AudioPlayerInterface
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

export class [[nodiscard]] AudioPlayer final : public AudioPlayerInterface
{
public:
    AudioPlayer(
        const std::size_t channelCount,
        mem::Rc<const dgm::ResourceManager> resmgr) noexcept
        : channels(channelCount), resmgr(resmgr)
    {
    }

    ~AudioPlayer()
    {
        for (auto& channel : channels)
            channel.stop();
    }

public:
    void playSoundOnChannel(
        const std::string& soundName,
        const std::size_t channel,
        const bool force = false,
        std::optional<sf::Vector2f> position = std::nullopt) override
    {
        assert(channel < channels.size());
        if (force || !isChannelActive(channel))
        {
            channels[channel].setBuffer(
                resmgr->get<sf::SoundBuffer>(soundName).value());

            channels[channel].setRelativeToListener(position.has_value());
            position.and_then(
                [this, channel](
                    const sf::Vector2f& position) -> std::optional<sf::Vector2f>
                {
                    channels[channel].setPosition(
                        sf::Vector3f(position.x, position.y, 0.f));
                    return position;
                });

            channels[channel].play();
        }
    }

    void stopChannel(const std::size_t channel)
    {
        assert(channel < channels.size());
        channels[channel].stop();
    }

    void setSoundVolume(const float volume)
    {
        for (auto& channel : channels)
        {
            channel.setVolume(volume);
            channel.setAttenuation(1.f / 4);
        }
    }

private:
    std::vector<sf::Sound> channels;
    mem::Rc<const dgm::ResourceManager> resmgr;

    [[nodiscard]] bool isChannelActive(const std::size_t channel) const
    {
        return channels[channel].getStatus() == sf::Sound::Status::Playing;
    }
};
