#pragma once

#include "AudioPlayerInterface.hpp"
#include "events/AudioEvents.hpp"
#include <DGM/DGM.hpp>
#include <Memory.hpp>
#include <core/Scene.hpp>

class [[nodiscard]] AudioEngine final
{
public:
    [[nodiscard]] AudioEngine(
        mem::Rc<AudioPlayerInterface> audioPlayer, Scene& scene) noexcept
        : audioPlayer(audioPlayer), scene(scene)
    {
    }

    AudioEngine(AudioEngine&&) = default;
    AudioEngine(const AudioEngine&) = delete;

public: // Must visit on all related events
    void operator()(const SoundTriggeredAudioEvent&);

public:
    void update(const float);

private:
    [[nodiscard]] sf::Vector2f
    getRelativePosition(const sf::Vector2f& position) const;

    [[nodiscard]] std::optional<sf::Vector2f>
    getRelativePosition(bool confirmedPov, const sf::Vector2f& position) const;

    [[nodiscard]] bool isPovStateIndex(PlayerStateIndexType stateIdx) const;

private:
    size_t getAmbientChannelIndex();
    size_t getPovChannelIndex();

private:
    mem::Rc<AudioPlayerInterface> audioPlayer;
    Scene& scene;
    int povChannelIndex = 0;
    int ambientChannelIndex = 0;
};
