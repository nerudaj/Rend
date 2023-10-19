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

void AudioEngine::operator()(const LaserCrossbowAudioEvent& e)
{
    audioPlayer->playSoundOnChannel("lasercrossbow_fire.wav", e.channel, true);
}

void AudioEngine::operator()(const LaserDartBouncedAudioEvent& e)
{
    audioPlayer->playSoundOnChannel("laserdart_bounce.wav", 5, true);
}

void AudioEngine::operator()(const ExplosionTriggeredAudioEvent&)
{
    audioPlayer->playSoundOnChannel("explosion.wav", 5, true);
}

void AudioEngine::update(const float) {}
