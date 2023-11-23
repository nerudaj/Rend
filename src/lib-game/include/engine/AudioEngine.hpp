#pragma once

#include "audio/AudioPlayer.hpp"
#include "events/AudioEvents.hpp"
#include <DGM/DGM.hpp>
#include <core/Scene.hpp>

import Memory;

class AudioEngine final
{
public:
    [[nodiscard]] AudioEngine(
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<AudioPlayer> audioPlayer,
        Scene& scene) noexcept
        : resmgr(resmgr), audioPlayer(audioPlayer), scene(scene)
    {
    }

public: // Must visit on all related events
    void operator()(const LaserDartBouncedAudioEvent&);
    void operator()(const PickablePickedUpAudioEvent&);
    void operator()(const SoundTriggeredAudioEvent&);

public:
    void update(const float);

private:
    [[nodiscard]] sf::Vector2f
    getRelativePosition(const sf::Vector2f& position) const;

    [[nodiscard]] std::optional<sf::Vector2f> getRelativePosition(
        PlayerStateIndexType stateIdx, const sf::Vector2f& position) const;

    [[nodiscard]] bool isPovStateIndex(PlayerStateIndexType stateIdx) const;

private:
    size_t getAmbientChannelIndex();
    size_t getPovChannelIndex();

private:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<AudioPlayer> audioPlayer;
    Scene& scene;
    int povChannelIndex = 0;
    int ambientChannelIndex = 0;
};
