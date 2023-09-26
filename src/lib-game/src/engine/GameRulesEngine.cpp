#include "engine/GameRulesEngine.hpp"
#include "events/EventQueue.hpp"

void GameRulesEngine::operator()(const PickablePickedUpGameEvent& e)
{
    scene.things.eraseAtIndex(e.id);
}

void GameRulesEngine::update(const dgm::Time&)
{
    for (auto&& [thing, id] : scene.things)
    {
        if (thing.typeId != EntityType::Player) continue;

        scene.spatialIndex.removeFromLookup(id, thing.hitbox);

        auto&& candidates =
            scene.spatialIndex.getOverlapCandidates(thing.hitbox);
        for (auto&& candidateId : candidates)
        {
            auto&& candidate = scene.things[candidateId];
            if (isPickable(candidate.typeId)
                && dgm::Collision::basic(thing.hitbox, candidate.hitbox))
            {
                handleGrabbedPickable(thing, candidate, candidateId);
            }
        }

        scene.spatialIndex.returnToLookup(id, thing.hitbox);
    }
}

void GameRulesEngine::handleGrabbedPickable(
    Entity grabber, Entity pickup, std::size_t pickupId)
{
    if (give(grabber, pickup.typeId) && !isWeaponPickable(pickup.typeId))
    {
        EventQueue::add<PickablePickedUpGameEvent>(pickupId);
        scene.spatialIndex.removeFromLookup(pickupId, pickup.hitbox);
    }
}

bool GameRulesEngine::give(Entity& thing, EntityType pickupId)
{
    using enum EntityType;
    switch (pickupId)
    {
    case PickupHealth:
        if (thing.health == MAX_BASE_HEALTH) return false;
        thing.health = std::clamp(
            thing.health + MEDIKIT_HEALTH_AMOUNT, 0, MAX_BASE_HEALTH);
        break;
    case PickupArmor:
        if (thing.armor == MAX_ARMOR) return false;
        thing.armor =
            std::clamp(thing.armor + ARMORSHARD_ARMOR_AMOUNT, 0, MAX_ARMOR);
        break;
    case PickupMegaHealth:
        thing.health = std::clamp(
            thing.health + MEGAHEALTH_HEALTH_AMOUNT, 0, MAX_UPPED_HEALTH);
        break;
    case PickupMegaArmor:
        thing.armor =
            std::clamp(thing.health + MEGAARMOR_ARMOR_AMOUNT, 0, MAX_ARMOR);
        break;
    case PickupBullets:
        if (thing.bulletCount == MAX_BULLETS) return false;
        thing.bulletCount =
            std::clamp(thing.bulletCount + BULLET_AMOUNT, 0, MAX_BULLETS);
        break;
    case PickupShells:
        if (thing.shellCount == MAX_SHELLS) return false;
        thing.shellCount =
            std::clamp(thing.shellCount + SHELL_AMOUNT, 0, MAX_SHELLS);
        break;
    case PickupEnergy:
        if (thing.energyCount == MAX_ENERGY) return false;
        thing.energyCount =
            std::clamp(thing.energyCount + ENERGY_AMOUNT, 0, MAX_ENERGY);
        break;
    case PickupRockets:
        if (thing.rocketCount == MAX_ROCKETS) return false;
        thing.rocketCount =
            std::clamp(thing.rocketCount + ROCKET_AMOUNT, 0, MAX_ROCKETS);
        break;
    default:
        return false;
    }

    return true;
}
