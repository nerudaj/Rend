#include "engine/AudioEngine.hpp"

void AudioEngine::operator()(const ShotgunFiredAudioEvent& e)
{
    audioPlayer->playSoundOnChannel(
        "shotgun.wav", e.channel, true, getRelativePosition(e.position));
}

void AudioEngine::operator()(const BulletFiredAudioEvent& e)
{
    audioPlayer->playSoundOnChannel(
        "bullet.wav", e.channel, true, getRelativePosition(e.position));
}

void AudioEngine::operator()(const RocketFiredAudioEvent& e)
{
    audioPlayer->playSoundOnChannel(
        "flaregun.wav", e.channel, true, getRelativePosition(e.position));
}

void AudioEngine::operator()(const LaserCrossbowAudioEvent& e)
{
    audioPlayer->playSoundOnChannel(
        "lasercrossbow_fire.wav",
        e.channel,
        true,
        getRelativePosition(e.position));
}

void AudioEngine::operator()(const LaserDartBouncedAudioEvent& e)
{
    audioPlayer->playSoundOnChannel(
        "laserdart_bounce.wav", 5, true, getRelativePosition(e.position));
}

void AudioEngine::operator()(const ExplosionTriggeredAudioEvent& e)
{
    // TODO: play different sound for rocket, flare and dart
    audioPlayer->playSoundOnChannel(
        "explosion.wav", 5, true, getRelativePosition(e.position));
}

void AudioEngine::update(const float) {}

sf::Vector2f
AudioEngine::getRelativePosition(const sf::Vector2f& position) const
{
    return position - scene.things[scene.cameraAnchorIdx].hitbox.getPosition();
}
