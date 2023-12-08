#include <GameLoop.hpp>
#include <builder/SceneBuilder.hpp>
#include <catch.hpp>
#include <core/Scene.hpp>
#include <engine/AnimationEngine.hpp>
#include <engine/GameRulesEngine.hpp>
#include <events/EventQueue.hpp>

class [[nodiscard]] AudioEventVisitor final
{
public:
    void operator()(SoundTriggeredAudioEvent& e)
    {
        soundTriggered = e.sound == expectedSound;
        ++playedSoundCount;
    }

    std::string expectedSound = "";
    bool soundTriggered = false;
    unsigned playedSoundCount = 0;
};

import Memory;

TEST_CASE("[GameRuleEngine]")
{
    auto&& meshSize = sf::Vector2u(3u, 3u);
    auto&& voxelSize = sf::Vector2u(16u, 16u);
    auto&& mesh = dgm::Mesh(
        std::vector<int> { 1, 1, 1, 1, 0, 1, 1, 1, 1 }, meshSize, voxelSize);
    Scene scene = { .spatialIndex = dgm::SpatialIndex<EntityIndexType>(
                        dgm::Rect(0.f, 0.f, 48.f, 48.f), 16),
                    .distanceIndex = DistanceIndex(mesh),
                    .navmesh = dgm::WorldNavMesh(mesh) };

    auto&& eventQueue = mem::Rc<EventQueue>();
    auto&& gameRulesEngine = GameRulesEngine(scene, eventQueue);
    auto&& animationEngine = AnimationEngine(scene, eventQueue);

    auto simulateStep = [&]()
    {
        const float FRAME_TIME = 1.f / 60.f;
        animationEngine.update(FRAME_TIME);
        gameRulesEngine.update(FRAME_TIME);
        eventQueue->processAndClear(animationEngine);
        eventQueue->processAndClear(gameRulesEngine);
        gameRulesEngine.deleteMarkedObjects();
        scene.tick++;
    };

    SECTION("BUG: Explosion sound was not played")
    {
        // Insert projectile and destroy it
        scene.things.emplaceBack(SceneBuilder::createProjectile(
            EntityType::ProjectileRocket, Position {}, Direction {}, 0, 0));
        eventQueue->emplace<ProjectileDestroyedGameEvent>(0);

        // After one step, explosion should be created
        simulateStep();
        REQUIRE(scene.things.isIndexValid(1));
        REQUIRE(scene.things[1].typeId == EntityType::EffectRocketExplosion);

        // After another step, sound playback should be triggered
        simulateStep();
        auto&& audioEventVisitor = AudioEventVisitor {};
        audioEventVisitor.expectedSound = "rocket_explosion.wav";
        eventQueue->processAndClear(audioEventVisitor);
        REQUIRE(audioEventVisitor.soundTriggered);
        REQUIRE(audioEventVisitor.playedSoundCount++);
    }
}
