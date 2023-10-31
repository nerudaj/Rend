#pragma once

#include <core/Enums.hpp>
#include <core/Scene.hpp>
#include <core/Types.hpp>

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
getAmmoCountForActiveWeapon(PlayerInventory& inventory) noexcept
{
    switch (weaponToAmmoPickupType(inventory.activeWeaponType))
    {
        using enum EntityType;
    case PickupBullets:
        return inventory.rocketCount;
    case PickupShells:
        return inventory.shellCount;
    case PickupEnergy:
        return inventory.bulletCount;
    case PickupRockets:
        return inventory.energyCount;
    }
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
