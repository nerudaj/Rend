#pragma once

#include <DGM/fsm.hpp>
#include <core/Enums.hpp>
#include <core/Scene.hpp>

import Memory;

class AiEngine final
{
public:
    [[nodiscard]] AiEngine(Scene& scene);

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

private:
    void discoverInterestingLocation();

    constexpr void doNothing(AiBlackboard&) noexcept {}

    void pickJumpPoint(AiBlackboard& blackboard);

    bool isJumpPointReached(const AiBlackboard&) const;

    void goToJumpPoint(AiBlackboard& blackboard);

    void shoot(AiBlackboard& blackboard)
    {
        blackboard.input->shoot();
    }

    [[nodiscard]] constexpr bool
    isPlayerAlive(const AiBlackboard& blackboard) const noexcept
    {
        return scene.things.isIndexValid(getInventory(blackboard).ownerIdx);
    }

    [[nodiscard]] constexpr auto&&
    getInventory(this auto&& self, const AiBlackboard& blackboard)
    {
        return self.scene.playerStates[blackboard.playerStateIdx].inventory;
    }

    [[nodiscard]] constexpr auto&&
    getPlayer(this auto&& self, const AiBlackboard& blackboard)
    {
        return self.scene.things[self.getInventory(blackboard).ownerIdx];
    }

private:
    Scene& scene;
    dgm::WorldNavMesh navmesh;
    dgm::fsm::Fsm<AiBlackboard, AiState> fsm;
    std::vector<WeaponLocation> weaponLocations;
    std::vector<PowerItemLocation> powerItemLocations;
    std::vector<PickupLocation> pickupLocations;
};
