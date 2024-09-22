#pragma once

#include "ai/AiEnums.hpp"
#include <Memory.hpp>
#include <core/Enums.hpp>
#include <core/Scene.hpp>
#include <fsm/Fsm.hpp>
#include <ranges>
#include <utils/Hitscanner.hpp>

class [[nodiscard]] AiEngine final
{
public:
    [[nodiscard]] AiEngine(Scene& scene, bool useNullBehavior);

public:
    void update(const float deltaTime);

private:
    static fsm::Fsm<AiBlackboard>
    createFsm(AiEngine& self, bool useNullBehavior);

private: // FSM predicates
    [[nodiscard]] bool
    isTargetLocationReached(const AiBlackboard& blackboard) const noexcept;

    [[nodiscard]] constexpr bool
    isThisPlayerAlive(const AiBlackboard& blackboard) const noexcept
    {
        return isPlayerAlive(getInventory(blackboard).ownerIdx);
    }

    [[nodiscard]] constexpr bool
    isTargetEnemyDead(const AiBlackboard& blackboard) const noexcept
    {
        return !isPlayerAlive(blackboard.targetEnemyIdx);
    }

    [[nodiscard]] bool
    isTargetEnemyOutOfView(const AiBlackboard& blackboard) const noexcept;

    [[nodiscard]] bool
    isTargetEnemyInReticle(const AiBlackboard& blackboard) const noexcept;

    [[nodiscard]] bool
    isAnyEnemyVisible(const AiBlackboard& blackboard) const noexcept;

    [[nodiscard]] constexpr bool
    hasEnteredWeaponRaise(const AiBlackboard& blackboard) const noexcept
    {
        return getInventory(blackboard).animationContext.animationStateId
               == AnimationStateId::Raise;
    }

    [[nodiscard]] constexpr bool
    canShoot(const AiBlackboard& blackboard) const noexcept
    {
        return getInventory(blackboard).animationContext.animationStateId
               == AnimationStateId::Idle;
    }

    [[nodiscard]] bool
    shouldSwapToLongRangeWeapon(const AiBlackboard& blackboard) const;

    [[nodiscard]] bool
    shouldSwapToShortRangeWeapon(const AiBlackboard& blackboard) const;

    [[nodiscard]] constexpr bool isActiveWeaponFlaregunAndLastSomethingElse(
        const AiBlackboard& blackboard) const noexcept
    {
        const auto& inventory = getInventory(blackboard);
        return inventory.activeWeaponType == EntityType::WeaponFlaregun
               && inventory.activeWeaponType != inventory.lastWeaponType;
    }

    [[nodiscard]] constexpr bool isTargetWeaponHighlightedInInventory(
        const AiBlackboard& blackboard) const noexcept
    {
        return getInventory(blackboard).selectionIdx
               == weaponTypeToIndex(blackboard.targetWeaponToSwapTo);
    }

    [[nodiscard]] bool
    isTooCloseWithLongRangeWeapon(const AiBlackboard& blackboard)
    {
        return isLongRangeWeaponType(getInventory(blackboard).activeWeaponType)
               && dgm::Math::getSize(
                      getEnemy(blackboard).hitbox.getPosition()
                      - getPlayer(blackboard).hitbox.getPosition())
                      < 32.f;
    }

    [[nodiscard]] bool
    isTooFarWithShortRangeWeapon(const AiBlackboard& blackboard)
    {
        return getInventory(blackboard).activeWeaponType
                   == EntityType::WeaponShotgun
               && dgm::Math::getSize(
                      getEnemy(blackboard).hitbox.getPosition()
                      - getPlayer(blackboard).hitbox.getPosition())
                      > 32.f;
    }

    [[nodiscard]] bool
    hasNoAmmoForActiveWeapon(const AiBlackboard& blackboard) const noexcept;

    [[nodiscard]] bool wasHurt(const AiBlackboard& blackboard) const noexcept
    {
        return blackboard.personality != AiPersonality::Tank
               && getPlayer(blackboard).health < blackboard.lastHealth;
    }

private: // FSM actions
    void pickGatherLocation(AiBlackboard& blackboard);

    void pickTargetEnemy(AiBlackboard& blackboard) noexcept;

    void moveTowardTargetLocation(AiBlackboard& blackboard);

    void moveInRelationToTargetEnemy(AiBlackboard& blackboard);

    void rotateTowardTargetLocation(AiBlackboard& blackboard)
    {
        rotateTowardTarget(
            getPlayer(blackboard), blackboard.input, blackboard.targetLocation);
    }

    void rotateTowardTargetEnemy(AiBlackboard& blackboard)
    {
        rotateTowardTarget(
            getPlayer(blackboard),
            blackboard.input,
            getEnemy(blackboard).hitbox.getPosition());
    }

    void shoot(AiBlackboard& blackboard) const noexcept
    {
        blackboard.input->shoot();
    }

    void confirmWeaponSelection(AiBlackboard& blackboard) const
    {
        // blackboard.delayedTransitionState = AiState::ChoosingGatherLocation;
        shoot(blackboard);
    }

    void performComboSwap(AiBlackboard& blackboard) const noexcept
    {
        blackboard.input->switchToLastWeapon();
    }

    void selectNextWeapon(AiBlackboard& blackboard) const
    {
        blackboard.input->switchToNextWeapon();
    }

    constexpr void
    pickTargetShortRangeWeapon(AiBlackboard& blackboard) const noexcept
    {
        blackboard.targetWeaponToSwapTo = EntityType::WeaponShotgun;
    }

    constexpr void
    pickTargetLongRangeWeapon(AiBlackboard& blackboard) const noexcept
    {
        const auto& inventory = getInventory(blackboard);
        for (auto&& type :
             getLongRangeWeaponTypesSortedByPersonality(blackboard.personality))
        {
            if (inventory.acquiredWeapons[weaponTypeToIndex(type)])
            {
                std::cout << "Switching to " << std::to_underlying(type)
                          << std::endl;
                blackboard.targetWeaponToSwapTo = type;
                return;
            }
        }
    }

    constexpr void performHuntBookmarking(AiBlackboard& blackboard)
    {
        // blackboard.delayedTransitionState = AiState::LockingTarget;
        blackboard.targetLocation = getEnemy(blackboard).hitbox.getPosition();
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
        const Entity& player,
        mem::Rc<AiController>& input,
        const sf::Vector2f& targetLocation);

    [[nodiscard]] static constexpr std::array<EntityType, 4>
    getLongRangeWeaponTypesSortedByPersonality(AiPersonality personality)
    {
        switch (personality)
        {
        case AiPersonality::Tank:
            return { EntityType::WeaponCrossbow,
                     EntityType::WeaponLauncher,
                     EntityType::WeaponTrishot,
                     EntityType::WeaponBallista };
        case AiPersonality::Flash:
            return { EntityType::WeaponLauncher,
                     EntityType::WeaponCrossbow,
                     EntityType::WeaponBallista,
                     EntityType::WeaponTrishot };
        case AiPersonality::Speartip:
        default:
            return { EntityType::WeaponTrishot,
                     EntityType::WeaponCrossbow,
                     EntityType::WeaponLauncher,
                     EntityType::WeaponBallista };
        }
    }

    [[nodiscard]] static constexpr bool isLongRangeWeaponType(EntityType type)
    {
        return std::ranges::contains(
            getLongRangeWeaponTypesSortedByPersonality(AiPersonality::Default),
            type);
    }

private:
    Scene& scene;
    Hitscanner hitscanner;
    fsm::Fsm<AiBlackboard> fsm;
};
