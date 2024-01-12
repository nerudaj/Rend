#pragma once

#include <DGM/fsm.hpp>
#include <core/Enums.hpp>
#include <core/Scene.hpp>
#include <ranges>
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

    [[nodiscard]] constexpr bool isTargetEnemyDead(
        const AiBlackboard&,
        const Entity&,
        const PlayerInventory&) const noexcept;

    [[nodiscard]] bool isTargetEnemyOutOfView(
        const AiBlackboard& blackboard,
        const Entity& player,
        const PlayerInventory& inventory) const noexcept;

    [[nodiscard]] bool isTargetEnemyInReticle(
        const AiBlackboard& blackboard,
        const Entity& player,
        const PlayerInventory& inventory) const noexcept;

    [[nodiscard]] bool isAnyEnemyVisible(
        const AiBlackboard& blackboard,
        const Entity& player,
        const PlayerInventory& inventory) const noexcept;

    [[nodiscard]] constexpr bool hasEnteredWeaponRaise(
        const AiBlackboard&,
        const Entity&,
        const PlayerInventory& inventory) const noexcept
    {
        return inventory.animationContext.animationStateId
               == AnimationStateId::Raise;
    }

    [[nodiscard]] constexpr bool canShoot(
        const AiBlackboard&,
        const Entity&,
        const PlayerInventory& inventory) const noexcept
    {
        return inventory.animationContext.animationStateId
               == AnimationStateId::Idle;
    }

    [[nodiscard]] bool shouldSwapToLongRangeWeapon(
        const AiBlackboard&,
        const Entity&,
        const PlayerInventory& inventory) const;

    [[nodiscard]] bool shouldSwapToShortRangeWeapon(
        const AiBlackboard&,
        const Entity&,
        const PlayerInventory& inventory) const;

    [[nodiscard]] constexpr bool isActiveWeaponFlaregunAndLastSomethingElse(
        const AiBlackboard&,
        const Entity&,
        const PlayerInventory& inventory) const noexcept
    {
        return inventory.activeWeaponType == EntityType::WeaponFlaregun
               && inventory.activeWeaponType != inventory.lastWeaponType;
    }

    [[nodiscard]] constexpr bool isTargetWeaponHighlightedInInventory(
        const AiBlackboard& blackboard,
        const Entity&,
        const PlayerInventory& inventory) const noexcept
    {
        return inventory.selectionIdx
               == weaponTypeToIndex(blackboard.targetWeaponToSwapTo);
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
        AiBlackboard& blackboard, Entity& player, PlayerInventory&)
    {
        rotateTowardTarget(player, blackboard.input, blackboard.targetLocation);
    }

    void rotateTowardTargetEnemy(
        AiBlackboard& blackboard, Entity& player, PlayerInventory&)
    {
        rotateTowardTarget(
            player,
            blackboard.input,
            getEnemy(blackboard).hitbox.getPosition());
    }

    void
    shoot(AiBlackboard& blackboard, Entity&, PlayerInventory&) const noexcept
    {
        blackboard.input->shoot();
    }

    void confirmWeaponSelection(
        AiBlackboard& blackboard,
        Entity& entity,
        PlayerInventory& inventory) const
    {
        blackboard.delayedTransitionState = AiState::Gathering;
        shoot(blackboard, entity, inventory);
    }

    void performComboSwap(
        AiBlackboard& blackboard, Entity&, PlayerInventory&) const noexcept
    {
        blackboard.input->switchToLastWeapon();
    }

    void
    selectNextWeapon(AiBlackboard& blackboard, Entity&, PlayerInventory&) const
    {
        blackboard.input->switchToNextWeapon();
    }

    constexpr void pickTargetShortRangeWeapon(
        AiBlackboard& blackboard, Entity&, PlayerInventory&) const noexcept
    {
        blackboard.targetWeaponToSwapTo = EntityType::WeaponShotgun;
    }

    constexpr void pickTargetLongRangeWeapon(
        AiBlackboard& blackboard,
        Entity&,
        PlayerInventory& inventory) const noexcept
    {
        for (auto&& type : getLongRangeWeaponTypes())
        {
            if (inventory.acquiredWeapons[weaponTypeToIndex(type)])
            {
                blackboard.targetWeaponToSwapTo = type;
                return;
            }
        }
    }

private: // Utility predicates
    [[nodiscard]] constexpr bool
    isPlayerAlive(EntityIndexType idx) const noexcept
    {
        return scene.things.isIndexValid(idx)
               && scene.things[idx].typeId == EntityType::Player;
    }

    [[nodiscard]] constexpr bool
    hasLongRangeWeapon(const PlayerInventory& inventory) const noexcept
    {
        return inventory.acquiredWeapons[weaponTypeToIndex(
                   EntityType::WeaponTrishot)]
               || inventory.acquiredWeapons[weaponTypeToIndex(
                   EntityType::WeaponCrossbow)]
               || inventory.acquiredWeapons[weaponTypeToIndex(
                   EntityType::WeaponLauncher)]
               || inventory.acquiredWeapons[weaponTypeToIndex(
                   EntityType::WeaponBallista)];
    }

    [[nodiscard]] constexpr bool
    hasShortRangeWeapon(const PlayerInventory& inventory) const noexcept
    {
        return inventory
            .acquiredWeapons[weaponTypeToIndex(EntityType::WeaponShotgun)];
    }

    [[nodiscard]] bool isEnemyVisible(
        const Entity& player,
        EntityIndexType playerIdx,
        EntityIndexType enemyIdx,
        const sf::Vector2f& enemyPosition) const noexcept;

private: // Utility functions
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

    [[nodiscard]] constexpr auto&&
    getEnemy(this auto&& self, const AiBlackboard& blackboard) noexcept
    {
        return self.scene.things[blackboard.targetEnemyIdx];
    }

    [[nodiscard]] sf::Vector2f getDirectionToEnemy(
        const sf::Vector2f& myPosition, EntityIndexType enemyIdx) const noexcept
    {
        return dgm::Math::toUnit(
            scene.things[enemyIdx].hitbox.getPosition() - myPosition);
    }

    void rotateTowardTarget(
        Entity& player,
        mem::Rc<AiController>& input,
        const sf::Vector2f& targetLocation);

    [[nodiscard]] static constexpr std::array<EntityType, 4>
    getLongRangeWeaponTypes()
    {
        return { EntityType::WeaponTrishot,
                 EntityType::WeaponCrossbow,
                 EntityType::WeaponLauncher,
                 EntityType::WeaponLauncher };
    }

    [[nodiscard]] static constexpr bool isLongRangeWeaponType(EntityType type)
    {
        return std::ranges::contains(getLongRangeWeaponTypes(), type);
    }

private:
    Scene& scene;
    Hitscanner hitscanner;
    dgm::fsm::Fsm<AiTopState, AiBlackboard> fsmTop;
    dgm::fsm::Fsm<AiState, AiBlackboard, Entity, PlayerInventory> fsmAlive;
    dgm::fsm::Fsm<AiState, AiBlackboard> fsmDead;
};
