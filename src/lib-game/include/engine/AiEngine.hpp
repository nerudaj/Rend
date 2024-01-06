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

public:
    void update(const float deltaTime);

private: // fsm updates
    void runFsmAlive(AiBlackboard& blackboard);

    void runFsmDead(AiBlackboard& blackboard);

private:
    static dgm::fsm::Fsm<AiTopState, AiBlackboard> createTopFsm(AiEngine& self);

    static dgm::fsm::Fsm<AiState, AiBlackboard, Entity, PlayerInventory>
    createAliveFsm(AiEngine& self);

    static dgm::fsm::Fsm<AiState, AiBlackboard> createDeadFsm(AiEngine& self);

private: // FSM predicates
    [[nodiscard]] bool isTargetLocationReached(
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

private: // FSM actions
    void pickGatherLocation(
        AiBlackboard& blackboard, Entity& player, PlayerInventory& inventory);

    void pickTargetEnemy(
        AiBlackboard& blackboard,
        Entity& player,
        PlayerInventory& inventory) noexcept;

    void moveTowardTargetLocation(
        AiBlackboard& blackboard, Entity& player, PlayerInventory& inventory);

    void rotateTowardTargetLocation(
        AiBlackboard& blackboard, Entity& player, PlayerInventory& inventory);

    void
    shoot(AiBlackboard& blackboard, Entity&, PlayerInventory&) const noexcept
    {
        blackboard.input->shoot();
    }

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
            != blackboard.targetEnemyIdx);
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

private:
    Scene& scene;
    Hitscanner hitscanner;
    dgm::fsm::Fsm<AiTopState, AiBlackboard> fsmTop;
    dgm::fsm::Fsm<AiState, AiBlackboard, Entity, PlayerInventory> fsmAlive;
    dgm::fsm::Fsm<AiState, AiBlackboard> fsmDead;
};
