#include "engine/AudioEngine.hpp"

constexpr unsigned PLAYER_1_CHANNEL = 0;
constexpr unsigned AMBIENT_1_CHANNEL = 4;
constexpr unsigned POV_CHANNEL = 8;
constexpr unsigned POV_CHANNEL_COUNT = 2;
constexpr unsigned AMBIENT_CHANNEL_COUNT = 4;

void AudioEngine::operator()(const SoundTriggeredAudioEvent& e)
{
    if (e.sourceType == SoundSourceType::Pov && !isPovStateIndex(e.stateIdx))
        return;

    auto channelIdx = [&]
    {
        if (e.sourceType == SoundSourceType::Pov)
            return getPovChannelIndex();
        else if (e.sourceType == SoundSourceType::Ambient)
            return getAmbientChannelIndex();
        return PLAYER_1_CHANNEL + e.stateIdx;
    }();

    audioPlayer->playSoundOnChannel(
        e.sound, channelIdx, true, getRelativePosition(e.stateIdx, e.position));
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

size_t AudioEngine::getAmbientChannelIndex()
{
    ambientChannelIndex = (ambientChannelIndex + 1) % AMBIENT_CHANNEL_COUNT;
    return AMBIENT_CHANNEL_COUNT + ambientChannelIndex;
}

size_t AudioEngine::getPovChannelIndex()
{
    povChannelIndex = (povChannelIndex + 1) % POV_CHANNEL_COUNT;
    return POV_CHANNEL + povChannelIndex;
}
