#include "audio/AudioPlayer.hpp"

void AudioPlayer::playSoundOnChannel(
    const std::string& soundName, const unsigned channel, const bool force)
{
    if (force || !isChannelActive(channel))
    {
        channels[channel].setBuffer(
            resmgr->get<sf::SoundBuffer>(soundName).value());
        channels[channel].play();
    }
}

void AudioPlayer::stopChannel(const unsigned channel)
{
    channels[channel].stop();
}

void AudioPlayer::setSoundVolume(const float volume)
{
    for (auto& channel : channels)
        channel.setVolume(volume);
}

AudioPlayer::~AudioPlayer()
{
    for (auto& channel : channels)
        channel.stop();
}