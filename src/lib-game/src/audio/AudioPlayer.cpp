#include "audio/AudioPlayer.hpp"

void AudioPlayer::playSoundOnChannel(
    const std::string& soundName,
    const std::size_t channel,
    const bool force,
    const sf::Vector2f& position)
{
    if (force || !isChannelActive(channel))
    {
        channels[channel].setBuffer(
            resmgr->get<sf::SoundBuffer>(soundName).value());
        channels[channel].setPosition(
            sf::Vector3f(position.x, position.y, 0.f));
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
        channel.setAttenuation(0.1f);
    }
}

AudioPlayer::~AudioPlayer()
{
    for (auto& channel : channels)
        channel.stop();
}