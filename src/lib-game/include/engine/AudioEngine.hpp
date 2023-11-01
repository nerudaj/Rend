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
    void operator()(const FlaregunFiredAudioEvent&);
    void operator()(const ShotgunFiredAudioEvent&);
    void operator()(const BulletFiredAudioEvent&);
    void operator()(const RocketFiredAudioEvent&);
    void operator()(const LaserCrossbowAudioEvent&);
    void operator()(const LaserDartBouncedAudioEvent&);
    void operator()(const ExplosionTriggeredAudioEvent&);
    void operator()(const PickablePickedUpAudioEvent&);
    void operator()(const WeaponRecoveringAudioEvent&);

public:
    void update(const float deltaTime);

private:
    [[nodiscard]] sf::Vector2f
    getRelativePosition(const sf::Vector2f& position) const;

private:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<AudioPlayer> audioPlayer;
    Scene& scene;
};
