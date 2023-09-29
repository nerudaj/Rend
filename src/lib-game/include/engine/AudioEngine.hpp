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
    void operator()(const DummyAudioEvent&) {}

public:
    void update(const float deltaTime);

private:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<AudioPlayer> audioPlayer;
};
