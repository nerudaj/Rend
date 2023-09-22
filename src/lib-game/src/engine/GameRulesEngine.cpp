#include "engine/GameRulesEngine.hpp"

void GameRulesEngine::update(const dgm::Time&)
{
    auto handlePlayer = [&](Player& actor, size_t id)
    {
        scene.things.removeFromLookup(id, actor.body);

        auto&& candidates = scene.things.getOverlapCandidates(actor.body);
        for (auto&& cid : candidates)
        {
            auto&& thing = scene.things[cid];

            std::visit(
                overloaded { [&](const GenericPickup& pickup)
                             {
                                 scene.things.eraseAtIndex(cid, pickup.body);
                                 give(actor, pickup.id);
                                 // TODO: schedule respawn
                             },
                             [](const WeaponPickup& pickup) {},
                             [](const Projectile& projectile) {},
                             [](const BaseObject&) {} },
                thing);
        }

        scene.things.returnToLookup(id, actor.body);
    };

    for (auto&& [thing, id] : scene.things)
    {
        std::visit(
            overloaded { [&](Player& p) { handlePlayer(p, id); },
                         [](BaseObject&) {} },
            thing);
    }
}

void GameRulesEngine::give(Player& actor, PickupId pickup) {
    using enum PickupId;
    switch (pickup)
    {
    case Medkit:
        if (actor.health < MAX_BASE_HEALTH)
            actor.health = std::clamp(
                actor.health + MEDIKIT_HEALTH_AMOUNT, 0, MAX_BASE_HEALTH);
        break;
    case ArmorShard:
        actor.armor =
            std::clamp(actor.armor + ARMORSHARD_ARMOR_AMOUNT, 0, MAX_ARMOR);
        break;
    case MegaHealth:
        actor.health = std::clamp(
            actor.health + MEGAHEALTH_HEALTH_AMOUNT, 0, MAX_UPPED_HEALTH);
        break;
    case MegaArmor:
        actor.armor = std::clamp(actor.health + MEGAARMOR_ARMOR_AMOUNT, 0, MAX_ARMOR);
        break;
    case Bullets:
        if (actor.bulletCount < MAX_BULLETS)
            actor.bulletCount =
                std::clamp(actor.bulletCount + BULLET_AMOUNT, 0, MAX_BULLETS);
        break;
    case Shells:
        if (actor.bulletCount < MAX_SHELLS)
            actor.shellCount =
                std::clamp(actor.shellCount + SHELL_AMOUNT, 0, MAX_SHELLS);
        break;
    case EnergyPack:
        if (actor.energyCount < MAX_ENERGY)
            actor.energyCount =
                std::clamp(actor.energyCount + ENERGY_AMOUNT, 0, MAX_ENERGY);
        break;
    case Rockets:
        if (actor.rocketCount < MAX_ROCKETS)
            actor.rocketCount =
                std::clamp(actor.rocketCount + ROCKET_AMOUNT, 0, MAX_ROCKETS);
        break;
    }
}
