#pragma once

#include "audio/AudioPlayer.hpp"
#include "events/AudioEvents.hpp"
#include <DGM/DGM.hpp>

import Memory;

class AudioEngine final
{
public:
    [[nodiscard]] AudioEngine(
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<AudioPlayer> audioPlayer) noexcept
        : resmgr(resmgr), audioPlayer(audioPlayer)
    {
    }

public: // Must visit on all related events
    void operator()(const ShotgunFiredAudioEvent&);
    void operator()(const BulletFiredAudioEvent&);
    void operator()(const FlareFiredAudioEvent&);
    void operator()(const LaserCrossbowAudioEvent&);
    void operator()(const LaserDartBouncedAudioEvent&);

public:
    void update(const float deltaTime);

private:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<AudioPlayer> audioPlayer;
};
