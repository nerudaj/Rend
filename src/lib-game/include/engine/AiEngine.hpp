#pragma once

#include <DGM/fsm.hpp>
#include <core/Enums.hpp>
#include <core/Scene.hpp>
#include <utils/Hitscanner.hpp>

import Memory;

class AiEngine final
{
public:
    [[nodiscard]] AiEngine(Scene& scene);

private:
    static dgm::fsm::Fsm<AiTopState, AiBlackboard> createTopFsm(AiEngine& self);

    static dgm::fsm::Fsm<AiState, AiBlackboard, Entity, PlayerInventory>
    createAliveFsm(AiEngine& self);

    static dgm::fsm::Fsm<AiState, AiBlackboard> createDeadFsm(AiEngine& self);

public:
    void update(const float deltaTime);

private:
    struct WeaponLocation
    {
        WeaponIndexType weaponIndex;
        sf::Vector2f position;
    };

    struct PowerItemLocation
    {
        EntityType type;
        sf::Vector2f position;
    };

    struct PickupLocation
    {
        EntityType type;
        sf::Vector2f position;
    };

private: // FSM predicates
    [[nodiscard]] constexpr bool hasSeekTimerElapsed(
        const AiBlackboard& blackboard,
        const Entity&,
        const PlayerInventory&) const noexcept
    {
        return blackboard.seekTimeout == 0.f;
    }

    [[nodiscard]] bool isJumpPointReached(
        const AiBlackboard&,
        const Entity&,
        const PlayerInventory&) const noexcept;

    [[nodiscard]] constexpr bool
    isThisPlayerAlive(const AiBlackboard& blackboard) const noexcept
    {
        return isPlayerAlive(getInventory(blackboard).ownerIdx);
    }

    [[nodiscard]] bool isTrackedEnemyVisible(
        const AiBlackboard& blackboard,
        const Entity& player,
        const PlayerInventory& inventory) const noexcept;

    [[nodiscard]] bool isTargetInReticle(
        const AiBlackboard& blackboard,
        const Entity& player,
        const PlayerInventory& inventory) const noexcept;

    [[nodiscard]] bool canShoot(
        const AiBlackboard&,
        const Entity&,
        const PlayerInventory& inventory) const noexcept
    {
        return inventory.animationContext.animationStateId
               == AnimationStateId::Idle;
    }

    [[nodiscard]] bool shouldSwapWeapon(
        const AiBlackboard& blackboard,
        const Entity& player,
        const PlayerInventory& inventory) const noexcept;

private: // FSM actions
    constexpr void doNothing(AiBlackboard&) const noexcept {}

    void pickJumpPoint(
        AiBlackboard& blackboard, Entity& player, PlayerInventory& inventory);

    void pickTargetEnemy(
        AiBlackboard& blackboard,
        Entity& player,
        PlayerInventory& inventory) noexcept;

    void moveTowardsTarget(
        AiBlackboard& blackboard, Entity& player, PlayerInventory& inventory);

    void
    shoot(AiBlackboard& blackboard, Entity&, PlayerInventory&) const noexcept
    {
        blackboard.input->shoot();
    }

    void resetBlackboard(AiBlackboard& blackboard) const noexcept;

    void rotateTowardsEnemy(
        AiBlackboard& blackboard,
        Entity& player,
        PlayerInventory& inventory) noexcept;

    void swapWeapon(
        AiBlackboard& blackboard,
        Entity& player,
        PlayerInventory& inventory) noexcept;

private: // Utility predicates
    [[nodiscard]] constexpr auto&&
    getInventory(this auto&& self, const AiBlackboard& blackboard) noexcept
    {
        return self.scene.playerStates[blackboard.playerStateIdx].inventory;
    }

    [[nodiscard]] constexpr auto&&
    getPlayer(this auto&& self, const AiBlackboard& blackboard) noexcept
    {
        assert(
            self.getInventory(blackboard).ownerIdx
            != blackboard.trackedEnemyIdx);
        return self.scene.things[self.getInventory(blackboard).ownerIdx];
    }

    [[nodiscard]] constexpr bool
    isPlayerAlive(EntityIndexType idx) const noexcept
    {
        return scene.things.isIndexValid(idx)
               && scene.things[idx].typeId == EntityType::Player;
    }

    [[nodiscard]] bool isEnemyVisible(
        EntityIndexType myIdx,
        const sf::Vector2f& myPosition,
        EntityIndexType enemyIdx,
        const sf::Vector2f& enemyPosition) const noexcept;

private: // Utility functions
    [[nodiscard]] sf::Vector2f getDirectionToEnemy(
        const sf::Vector2f& myPosition, EntityIndexType enemyIdx) const noexcept
    {
        return dgm::Math::toUnit(
            scene.things[enemyIdx].hitbox.getPosition() - myPosition);
    }

    [[nodiscard]] int getItemBaseScore(
        EntityType type,
        int myHealth,
        const PlayerInventory& inventory) const noexcept;

private: // fsm updates
    void runFsmAlive(AiBlackboard& blackboard);

    void runFsmDead(AiBlackboard& blackboard);

private:
    Scene& scene;
    Hitscanner hitscanner;
    dgm::WorldNavMesh navmesh;
    dgm::fsm::Fsm<AiTopState, AiBlackboard> fsmTop;
    dgm::fsm::Fsm<AiState, AiBlackboard, Entity, PlayerInventory> fsmAlive;
    dgm::fsm::Fsm<AiState, AiBlackboard> fsmDead;
};
