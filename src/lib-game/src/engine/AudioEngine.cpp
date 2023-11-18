#include "engine/AudioEngine.hpp"

constexpr unsigned PLAYER_1_CHANNEL = 0;
constexpr unsigned AMBIENT_1_CHANNEL = 4;
constexpr unsigned POV_CHANNEL = 8;
constexpr unsigned POV_CHANNEL_COUNT = 2;
constexpr unsigned AMBIENT_CHANNEL_COUNT = 4;

void AudioEngine::operator()(const FlaregunFiredAudioEvent& e)
{
    audioPlayer->playSoundOnChannel(
        "flaregun_fire.wav",
        e.stateIdx,
        true,
        getRelativePosition(e.stateIdx, e.position));
}

void AudioEngine::operator()(const ShotgunFiredAudioEvent& e)
{
    audioPlayer->playSoundOnChannel(
        "shotgun.wav",
        e.stateIdx,
        true,
        getRelativePosition(e.stateIdx, e.position));
}

void AudioEngine::operator()(const BulletFiredAudioEvent& e)
{
    audioPlayer->playSoundOnChannel(
        "bullet.wav",
        e.stateIdx,
        true,
        getRelativePosition(e.stateIdx, e.position));
}

void AudioEngine::operator()(const RocketFiredAudioEvent& e)
{
    audioPlayer->playSoundOnChannel(
        "launcher_fire.wav",
        e.stateIdx,
        true,
        getRelativePosition(e.stateIdx, e.position));
}

void AudioEngine::operator()(const LaserCrossbowAudioEvent& e)
{
    audioPlayer->playSoundOnChannel(
        "lasercrossbow_fire.wav",
        e.stateIdx,
        true,
        getRelativePosition(e.stateIdx, e.position));
}

void AudioEngine::operator()(const LaserDartBouncedAudioEvent& e)
{
    audioPlayer->playSoundOnChannel(
        "dart_bounce.wav",
        AMBIENT_1_CHANNEL + ambientChannelIndex,
        true,
        getRelativePosition(e.position));
    updatAmbientChannelIndex();
}

void AudioEngine::operator()(const ExplosionTriggeredAudioEvent& e)
{
    const auto soundName =
        e.type == EntityType::ProjectileFlare       ? "flare_explosion.wav"
        : e.type == EntityType::ProjectileLaserDart ? "dart_explosion.wav"
                                                    : "rocket_explosion.wav";

    audioPlayer->playSoundOnChannel(
        soundName,
        AMBIENT_1_CHANNEL + ambientChannelIndex,
        true,
        getRelativePosition(e.position));
    updatAmbientChannelIndex();
}

void AudioEngine::operator()(const PickablePickedUpAudioEvent& e)
{
    if (!isPovStateIndex(e.stateIdx)) return;

    bool megaPickup = e.type == EntityType::PickupMegaHealth
                      || e.type == EntityType::PickupMegaArmor;
    audioPlayer->playSoundOnChannel(
        megaPickup ? "megapickup.wav" : "pickup.wav",
        POV_CHANNEL + povChannelIndex,
        true);
    updatePovChannelIndex();
}

void AudioEngine::operator()(const WeaponRecoveringAudioEvent& e)
{
    if (!isPovStateIndex(e.stateIdx)) return;

    audioPlayer->playSoundOnChannel(
        "ssg_reload2.wav", POV_CHANNEL + povChannelIndex, true);
    updatePovChannelIndex();
}

void AudioEngine::update(const float) {}

sf::Vector2f
AudioEngine::getRelativePosition(const sf::Vector2f& position) const
{
    const auto& povPosition =
        scene.things[scene.cameraAnchorIdx].hitbox.getPosition();

    return dgm::Math::toUnit(position - povPosition)
           * static_cast<float>(
               scene.distanceIndex.getDistance(position, povPosition));
}

std::optional<sf::Vector2f> AudioEngine::getRelativePosition(
    PlayerStateIndexType stateIdx, const sf::Vector2f& position) const
{
    if (isPovStateIndex(stateIdx)) return std::nullopt;
    return getRelativePosition(position);
}

bool AudioEngine::isPovStateIndex(PlayerStateIndexType stateIdx) const
{
    return scene.playerStates[stateIdx].inventory.ownerIdx
           == scene.cameraAnchorIdx;
}

void AudioEngine::updatAmbientChannelIndex()
{
    ambientChannelIndex = (ambientChannelIndex + 1) % AMBIENT_CHANNEL_COUNT;
}

void AudioEngine::updatePovChannelIndex()
{
    povChannelIndex = (povChannelIndex + 1) % POV_CHANNEL_COUNT;
}
