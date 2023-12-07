#pragma once

#include <core/EntityDefinitions.hpp>
#include <core/Enums.hpp>
#include <core/Scene.hpp>
#include <core/Types.hpp>

// TODO: remove this
[[nodiscard]] constexpr static EntityType
weaponToAmmoPickupType(EntityType type) noexcept
{
    // TODO: Move this to entity defintions?
    switch (type)
    {
        using enum EntityType;
    case WeaponFlaregun:
        return PickupEnergy;
    case WeaponShotgun:
        return PickupShells;
    case WeaponTrishot:
        return PickupBullets;
    case WeaponCrossbow:
        return PickupEnergy;
    case WeaponLauncher:
        return PickupRockets;
    case WeaponBallista:
        return PickupRockets;
    }
}

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
