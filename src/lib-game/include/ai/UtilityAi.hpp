#pragma once

#include <core/Constants.hpp>
#include <core/EntityDefinitions.hpp>
#include <core/Enums.hpp>
#include <core/Types.hpp>

class UtilityAi
{
public:
    [[nodiscard]] static int getPickupScore(
        EntityType pickup,
        int health,
        int armor,
        const AmmoArray& ammo,
        unsigned activeAmmoIndex,
        const AcquiredWeaponsArray& acquiredWeapons)
    {
        const auto& def = ENTITY_PROPERTIES.at(pickup);

        if (pickup == EntityType::PickupMegaHealth)
            return 15000;
        else if (pickup == EntityType::PickupHealth)
        {
            if (health >= 100) return -1;
            return 10000 / (health * health);
        }
        else if (pickup == EntityType::PickupMegaArmor)
            return 9000;
        else if (isAmmoPickup(pickup))
        {
            auto index = ammoPickupToAmmoIndex(pickup);
            if (ammo[index] >= AMMO_LIMIT[index])
                return -1;
            else
            {
                if (index == activeAmmoIndex)
                    return 5000;
                else if (acquiredWeapons.to_ulong() == 1ul)
                    return 2500; // if only starter weapon was acquired
                else
                    return 1000;
            }
        }
        else if (def.traits & Trait::WeaponPickup)
        {
            auto index = weaponPickupToIndex(pickup);
            if (acquiredWeapons[index])
                return -1;
            else if (pickup == EntityType::PickupShotgun)
                return 7500;
            else
                return 6000;
        }
        else if (pickup == EntityType::PickupArmor)
        {
            if (armor >= 100)
                return -1;
            else if (armor == 0)
                return 2000;
            else
                return 1000 / armor;
        }

        return -1;
    }

private:
    [[nodiscard]] constexpr static bool isAmmoPickup(EntityType type)
    {
        return type == EntityType::PickupBullets
               || type == EntityType::PickupShells
               || type == EntityType::PickupEnergy
               || type == EntityType::PickupRockets;
    }
};