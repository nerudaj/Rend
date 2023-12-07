#pragma once

#include <core/EntityDefinitions.hpp>
#include <core/Enums.hpp>
#include <core/Scene.hpp>
#include <core/Types.hpp>

[[nodiscard]] constexpr static AmmoCounterType
getAmmoCountForActiveWeapon(const PlayerInventory& inventory) noexcept
{
    return inventory.ammo[ammoTypeToAmmoIndex(
        ENTITY_PROPERTIES.at(inventory.activeWeaponType).ammoType)];
}

[[nodiscard]] constexpr static int
getMinimumAmmoNeededToFireWeapon(EntityType type)
{
    // TODO: Move to entity definitions?
    switch (type)
    {
        using enum EntityType;
    case WeaponFlaregun:
        return 1;
    case WeaponShotgun:
        return 1;
    case WeaponTrishot:
        return 3;
    case WeaponCrossbow:
        return 1;
    case WeaponLauncher:
        return 1;
    case WeaponBallista:
        return 2;
    }
}

[[nodiscard]] static float getNormalizedDistanceToEpicenter(
    const dgm::Circle& entityHitbox, const dgm::Circle& explosionHitbox)
{
    return (dgm::Math::getSize(
                entityHitbox.getPosition() - explosionHitbox.getPosition())
            - entityHitbox.getRadius())
           / explosionHitbox.getRadius();
}
