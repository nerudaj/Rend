#include "Stubs/AudioPlayerStub.hpp"
#include "TestHelpers/SceneBuilder.hpp"
#include <builder/SceneBuilder.hpp>
#include <catch.hpp>
#include <engine/AudioEngine.hpp>
#include <fakeit.hpp>

using namespace fakeit;

TEST_CASE("[AudioEngine]")
{
    auto audioPlayerMock = mem::Rc<AudioPlayerStub>();
    auto audioPlayerSpy = Mock<AudioPlayerInterface>(*audioPlayerMock);
    auto&& mesh = createDummyMesh();
    Scene scene = createDummyScene(mesh);
    scene.things.emplaceBack(SceneBuilder::createPlayer(
        Position { sf::Vector2f(0.f, 0.f) },
        Direction { sf::Vector2f(1.f, 0.f) },
        0u));
    scene.playerStates.push_back(
        PlayerState { .inventory = PlayerInventory { .ownerIdx = 0 } });
    auto&& audioEngine = AudioEngine(audioPlayerMock, scene);

    SECTION("Construction from derived")
    {
        auto&& resmgr = mem::Rc<dgm::ResourceManager>();
        auto&& derived = mem::Rc<AudioPlayer>(1, resmgr);
        auto&& base = mem::Rc<AudioPlayerInterface>(derived);
    }

    SECTION("Attenuates non-pov non-player sounds")
    {
        When(Method(audioPlayerSpy, playSoundOnChannel)).Return();
        audioEngine.operator()(SoundTriggeredAudioEvent(
            "sound.wav",
            SoundSourceType::Ambient,
            0u,
            sf::Vector2f(100.f, 100.f)));
        Verify(
            Method(audioPlayerSpy, playSoundOnChannel)
                .Matching([](auto, auto, auto, std::optional<sf::Vector2f>& opt)
                          { return opt.has_value(); }))
            .Exactly(Once);
    }
}
