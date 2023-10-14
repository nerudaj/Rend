#include "engine/AudioEngine.hpp"

void AudioEngine::operator()(const ShotgunFiredAudioEvent& e)
{
    audioPlayer->playSoundOnChannel("shotgun.wav", e.channel, true);
}

void AudioEngine::operator()(const BulletFiredAudioEvent& e)
{
    audioPlayer->playSoundOnChannel("bullet.wav", e.channel, true);
}

void AudioEngine::operator()(const FlareFiredAudioEvent& e)
{
    audioPlayer->playSoundOnChannel("flaregun.wav", e.channel, true);
}

void AudioEngine::operator()(const LaserDartAudioEvent&)
{
    // audioPlayer->playSoundOnChannel("lasercrossbow.wav", 0, true);
}

void AudioEngine::update(const float) {}
