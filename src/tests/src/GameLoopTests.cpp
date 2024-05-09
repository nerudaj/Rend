#include "TestHelpers/SceneBuilder.hpp"
#include <GameLoop.hpp>
#include <Memory.hpp>
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

TEST_CASE("[GameRuleEngine]")
{
    auto&& mesh = createDummyMesh();
    Scene scene = createDummyScene(mesh);

    auto&& eventQueue = mem::Rc<EventQueue>();
    auto&& gameRulesEngine =
        GameRulesEngine(scene, eventQueue, { "", "", "", "" });
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
        REQUIRE(audioEventVisitor.playedSoundCount == 1u);
    }

    SECTION("BUG: fireRay does not drain energy")
    {
        scene.things.emplaceBack(createDummyPlayer());
        scene.playerStates.push_back(PlayerState {
            .inventory = SceneBuilder::getDefaultInventory(0u, 0) });
        scene.playerStates.front().inventory.activeWeaponType =
            EntityType::WeaponBallista;

        auto origBulletAmmo =
            scene.playerStates.front()
                .inventory.ammo[ammoTypeToAmmoIndex(AmmoType::Bullets)];
        auto origShellAmmo =
            scene.playerStates.front()
                .inventory.ammo[ammoTypeToAmmoIndex(AmmoType::Shells)];
        auto origRocketAmmo =
            scene.playerStates.front()
                .inventory.ammo[ammoTypeToAmmoIndex(AmmoType::Rockets)];
        auto origEnergyAmmo =
            scene.playerStates.front()
                .inventory.ammo[ammoTypeToAmmoIndex(AmmoType::Energy)];

        eventQueue->emplace<ScriptTriggeredGameEvent>(
            Script { .id = ScriptId::FireRay }, 0u);
        eventQueue->processAndClear(gameRulesEngine);

        auto&& ammo = scene.playerStates.front().inventory.ammo;
        REQUIRE(origBulletAmmo == ammo[ammoTypeToAmmoIndex(AmmoType::Bullets)]);
        REQUIRE(origShellAmmo == ammo[ammoTypeToAmmoIndex(AmmoType::Shells)]);
        REQUIRE(origRocketAmmo == ammo[ammoTypeToAmmoIndex(AmmoType::Rockets)]);
        REQUIRE(origEnergyAmmo > ammo[ammoTypeToAmmoIndex(AmmoType::Energy)]);
    }
}
