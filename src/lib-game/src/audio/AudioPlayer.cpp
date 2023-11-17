#include "audio/AudioPlayer.hpp"

void AudioPlayer::playSoundOnChannel(
    const std::string& soundName,
    const std::size_t channel,
    const bool force,
    std::optional<sf::Vector2f> position)
{
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

void AudioPlayer::stopChannel(const std::size_t channel)
{
    channels[channel].stop();
}

void AudioPlayer::setSoundVolume(const float volume)
{
    for (auto& channel : channels)
    {
        channel.setVolume(volume);
        channel.setAttenuation(1.f);
    }
}

AudioPlayer::~AudioPlayer()
{
    for (auto& channel : channels)
        channel.stop();
}