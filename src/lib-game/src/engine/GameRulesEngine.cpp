#include "engine/GameRulesEngine.hpp"
#include "events/EventQueue.hpp"

void GameRulesEngine::operator()(const PickablePickedUpGameEvent& e)
{
    scene.things.eraseAtIndex(e.id);
}

void GameRulesEngine::operator()(const ProjectileCreatedGameEvent& e)
{
    scene.things.emplaceBack(Scene::createProjectile(
        e.type, Position { e.position }, Direction { e.direction }));
}

void GameRulesEngine::operator()(const ProjectileDestroyedGameEvent& e)
{
    auto&& projectileHitbox = scene.things[e.entityIndex].hitbox;
    const auto&& explosionHitbox =
        dgm::Circle(projectileHitbox.getPosition(), EXPLOSION_RADIUS);

    // Delete projectile
    scene.spatialIndex.removeFromLookup(e.entityIndex, projectileHitbox);
    scene.things.eraseAtIndex(e.entityIndex);

    // Damage all destructibles in vicinity
    for (auto&& [candidateId, candidate] :
         getOverlapCandidates(explosionHitbox))
    {
        if (isDestructible(candidate.typeId)
            && dgm::Collision::basic(explosionHitbox, candidate.hitbox))
        {
            damage(candidate, candidateId, ROCKET_DAMAGE);
        }
    }

    // Spawn explosion effect
    scene.things.emplaceBack(Scene::createEffect(
        SpriteId::ExplosionA, Position { explosionHitbox.getPosition() }));
}

void GameRulesEngine::operator()(const EntityDestroyedGameEvent& e)
{
    auto&& thing = scene.things[e.entityIndex];
    const bool playerWasDestroyed = thing.typeId == EntityType::Player;

    if (playerWasDestroyed)
    {
        scene.things.emplaceBack(Scene::createEffect(
            SpriteId::DeathA, Position { thing.hitbox.getPosition() }));

        auto idx = scene.things.emplaceBack(
            Entity { .typeId = EntityType::MarkerDeadPlayer,
                     .spriteClipIndex = SpriteId::NoRender,
                     .hitbox = thing.hitbox,
                     .direction = thing.direction,
                     .inputId = thing.inputId });

        if (scene.playerId == e.entityIndex) scene.playerId = idx;
    }

    scene.spatialIndex.removeFromLookup(e.entityIndex, thing.hitbox);
    scene.things.eraseAtIndex(e.entityIndex);
}

void GameRulesEngine::update(const dgm::Time& time)
{
    for (auto&& [thing, id] : scene.things)
    {
        if (thing.typeId != EntityType::Player) continue;

        scene.spatialIndex.removeFromLookup(id, thing.hitbox);

        for (auto&& [candidateId, candidate] :
             getOverlapCandidates(thing.hitbox))
        {
            if (isPickable(candidate.typeId)
                && dgm::Collision::basic(thing.hitbox, candidate.hitbox))
            {
                handleGrabbedPickable(thing, candidate, candidateId);
            }
        }

        scene.spatialIndex.returnToLookup(id, thing.hitbox);

        if (thing.fireCooldown > 0.f)
            thing.fireCooldown -= time.getDeltaTime();
        else if (scene.inputs[thing.inputId].isShooting())
        {
            thing.fireCooldown = WEAPON_LAUNCHER_COOLDOWN;
            EventQueue::add<ProjectileCreatedGameEvent>(
                ProjectileCreatedGameEvent(
                    EntityType::ProjectileRocket,
                    thing.hitbox.getPosition()
                        + thing.direction * thing.hitbox.getRadius() * 2.f,
                    thing.direction));
        }
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

void GameRulesEngine::damage(Entity& thing, std::size_t thingIndex, int damage)
{
    // TODO: max 2/3 of armor above 100?
    // Max 1/3 of the damage can be absorbed by the armor
    int amountAbsorbedByArmor = std::min(thing.armor, damage / 3);
    thing.armor -= amountAbsorbedByArmor;
    damage -= amountAbsorbedByArmor;

    thing.health -= damage;
    if (thing.health <= 0)
    {
        EventQueue::add<EntityDestroyedGameEvent>(thingIndex);
    }
}
