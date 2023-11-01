#include "engine/AudioEngine.hpp"

void AudioEngine::operator()(const FlaregunFiredAudioEvent& e)
{
    audioPlayer->playSoundOnChannel(
        "flaregun_fire.wav", e.channel, true, getRelativePosition(e.position));
}

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
        "launcher_fire.wav", e.channel, true, getRelativePosition(e.position));
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
        "dart_bounce.wav", 5, true, getRelativePosition(e.position));
}

void AudioEngine::operator()(const ExplosionTriggeredAudioEvent& e)
{
    const auto soundName =
        e.type == EntityType::ProjectileFlare       ? "flare_explosion.wav"
        : e.type == EntityType::ProjectileLaserDart ? "dart_explosion.wav"
                                                    : "rocket_explosion.wav";

    audioPlayer->playSoundOnChannel(
        soundName, 5, true, getRelativePosition(e.position));
}

void AudioEngine::operator()(const PickablePickedUpAudioEvent& e)
{
    bool megaPickup = e.type == EntityType::PickupMegaHealth
                      || e.type == EntityType::PickupMegaArmor;
    audioPlayer->playSoundOnChannel(
        megaPickup ? "megapickup.wav" : "pickup.wav", e.channel, true);
}

void AudioEngine::operator()(const WeaponRecoveringAudioEvent& e)
{
    if (scene.playerStates[e.channel].inventory.ownerIdx
        != scene.cameraAnchorIdx)
        return;

    audioPlayer->playSoundOnChannel("ssg_reload2.wav", e.channel, true);
}

void AudioEngine::update(const float) {}

sf::Vector2f
AudioEngine::getRelativePosition(const sf::Vector2f& position) const
{
    return position - scene.things[scene.cameraAnchorIdx].hitbox.getPosition();
}
