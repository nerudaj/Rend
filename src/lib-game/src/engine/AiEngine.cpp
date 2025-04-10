﻿#include <ai/UtilityAi.hpp>
#include <core/EntityDefinitions.hpp>
#include <engine/AiEngine.hpp>
#include <utils/MathHelpers.hpp>

void AiEngine::update(const float deltaTime)
{
    for (auto&& state : scene.playerStates)
    {
        if (!state.blackboard.has_value()) continue;

        auto& blackboard = state.blackboard.value();
        blackboard.targettingTimer -= deltaTime;

        blackboard.input->clearInputs();
        fsm.tick(blackboard);
    }
}

bool AiEngine::isTargetLocationReached(
    const AiBlackboard& blackboard) const noexcept
{
    return dgm::Math::getSize(
               getPlayer(blackboard).hitbox.getPosition()
               - blackboard.targetLocation)
           < AI_MAX_POSITION_ERROR;
}

bool AiEngine::isTargetEnemyOutOfView(
    const AiBlackboard& blackboard) const noexcept
{
    if (!isPlayerAlive(blackboard.targetEnemyIdx)) return false;

    const auto& player = getPlayer(blackboard);
    const auto& inventory = getInventory(blackboard);
    const auto& enemy = scene.things[blackboard.targetEnemyIdx];

    assert(inventory.ownerIdx != blackboard.targetEnemyIdx);

    return !isEnemyVisible(
        player,
        inventory.ownerIdx,
        blackboard.targetEnemyIdx,
        enemy.hitbox.getPosition());
}

bool AiEngine::isTargetEnemyInReticle(
    const AiBlackboard& blackboard) const noexcept
{
    if (!isPlayerAlive(blackboard.targetEnemyIdx)) return false;

    const auto& player = getPlayer(blackboard);
    const auto dirToEnemy = getDirectionToEnemy(
        player.hitbox.getPosition(), blackboard.targetEnemyIdx);

    const float angle =
        std::acos(dgm::Math::getDotProduct(player.direction, dirToEnemy));
    return angle <= AI_MAX_AIM_ERROR;
}

bool AiEngine::isAnyEnemyVisible(const AiBlackboard& blackboard) const noexcept
{
    const auto& player = getPlayer(blackboard);
    const auto& inventory = getInventory(blackboard);

    return std::any_of(
        scene.playerStates.begin(),
        scene.playerStates.end(),
        [&](const PlayerState& state)
        {
            if (state.inventory.ownerIdx == inventory.ownerIdx
                || isSameTeam(state.inventory.team, inventory.team)
                || !isPlayerAlive(state.inventory.ownerIdx))
                return false;

            return isEnemyVisible(
                player,
                inventory.ownerIdx,
                state.inventory.ownerIdx,
                scene.things[state.inventory.ownerIdx].hitbox.getPosition());
        });
}

bool AiEngine::shouldSwapToLongRangeWeapon(const AiBlackboard& blackboard) const
{
    const auto& inventory = getInventory(blackboard);
    return blackboard.personality != AiPersonality::Speartip
           && hasLongRangeWeapon(inventory)
           && !isLongRangeWeaponType(inventory.activeWeaponType)
           && !isLongRangeWeaponType(inventory.lastWeaponType);
}

bool AiEngine::shouldSwapToShortRangeWeapon(
    const AiBlackboard& blackboard) const
{
    const auto& inventory = getInventory(blackboard);
    return hasShortRangeWeapon(inventory)
           && inventory.activeWeaponType != EntityType::WeaponShotgun
           && inventory.lastWeaponType != EntityType::WeaponShotgun;
}

bool AiEngine::hasNoAmmoForActiveWeapon(
    const AiBlackboard& blackboard) const noexcept
{
    const auto& inventory = getInventory(blackboard);
    return inventory.ammo[ammoTypeToAmmoIndex(
               ENTITY_PROPERTIES.at(inventory.activeWeaponType).ammoType)]
           < 3;
}

void AiEngine::pickGatherLocation(AiBlackboard& blackboard)
{
    const auto& player = getPlayer(blackboard);
    const auto& inventory = getInventory(blackboard);

    const auto activeAmmoIdx = ammoTypeToAmmoIndex(
        ENTITY_PROPERTIES.at(inventory.activeWeaponType).ammoType);

    auto&& bestPosition = sf::Vector2f(0.f, 0.f);
    float bestScore = 0;
    for (auto&& [thing, idx] : scene.things)
    {
        const auto& def = ENTITY_PROPERTIES.at(thing.typeId);
        if (!(def.traits & Trait::Pickable)) continue;

        const int distance = scene.distanceIndex.getDistance(
            player.hitbox.getPosition(), thing.hitbox.getPosition());
        const float score = UtilityAi::getPickupScore(
                                thing.typeId,
                                player.health,
                                player.armor,
                                inventory.ammo,
                                activeAmmoIdx,
                                inventory.acquiredWeapons,
                                preferFlags)
                            / static_cast<float>((distance * distance));

        if (score > bestScore)
        {
            bestScore = score;
            bestPosition = thing.hitbox.getPosition();
        }
    }

    // If fallback locations lies too close, pick new
    if (blackboard.longTermTargetLocation.x == 0.f
        || dgm::Math::getSize(
               player.hitbox.getPosition() - blackboard.longTermTargetLocation)
               < 16.f)
    {
        blackboard.longTermTargetLocation =
            scene.dummyAiDestinations
                [scene.tick % scene.dummyAiDestinations.size()];
    }

    // Fallback if no destination was found
    if (bestScore == 0 || std::isnan(bestScore) || std::isinf(bestScore))
    {
        bestPosition = blackboard.longTermTargetLocation;
    }

    setPathToTargetLocation(player, bestPosition, blackboard);
}

void AiEngine::pickTargetEnemy(AiBlackboard& blackboard) noexcept
{
    const auto& inventory = getInventory(blackboard);
    const auto& player = getPlayer(blackboard);

    for (auto&& state : scene.playerStates)
    {
        if (state.inventory.ownerIdx == inventory.ownerIdx) continue;

        // Don't consider teammates as enemies
        if (isSameTeam(state.inventory.team, inventory.team)) continue;

        if (!scene.things.isIndexValid(state.inventory.ownerIdx)
            || !isPlayer(scene.things[state.inventory.ownerIdx].typeId))
            continue;

        if (isEnemyVisible(
                player,
                inventory.ownerIdx,
                state.inventory.ownerIdx,
                scene.things[state.inventory.ownerIdx].hitbox.getPosition()))
        {
            blackboard.targetEnemyIdx = state.inventory.ownerIdx;
        }
    }
}

void AiEngine::moveTowardTargetLocation(AiBlackboard& blackboard)
{
    const auto& player = getPlayer(blackboard);
    const auto directionToDestination =
        blackboard.targetLocation - player.hitbox.getPosition();
    const float angle = dgm::Math::cartesianToPolar(player.direction).angle;
    const auto lookRelativeThrust = dgm::Math::getRotated(
        dgm::Math::toUnit(directionToDestination), -angle);

    blackboard.input->setThrust(lookRelativeThrust.x);
    blackboard.input->setSidewardThrust(lookRelativeThrust.y);
}

void AiEngine::moveInRelationToTargetEnemy(AiBlackboard& blackboard)
{
    if (blackboard.personality == AiPersonality::Speartip)
    {
        // Rush towards the target with a shotgun in hand
        blackboard.input->setThrust(0.6f);
    }
    else if (blackboard.personality == AiPersonality::Tank)
    {
        // do nothing, stand in place
    }
    else
    {
        // Back up from the fight slowly
        blackboard.input->setThrust(-0.3f);
        blackboard.input->setThrust(0.3f);
        // Circle strafe
        // blackboard.input->setSidewardThrust(0.5f);
    }
}

void AiEngine::setDestinationToFlagPole(AiBlackboard& blackboard)
{
    const auto& player = getPlayer(blackboard);

    for (const auto& [thing, idx] : scene.things)
    {

        if ((player.typeId == EntityType::CarrierRedPlayer
             && thing.typeId == EntityType::BlueFlag)
            || (player.typeId == EntityType::CarrierBluePlayer
                && thing.typeId == EntityType::RedFlag))
        {
            setPathToTargetLocation(
                player, thing.hitbox.getPosition(), blackboard);
            return;
        }
    }
}

bool AiEngine::isEnemyVisible(
    const Entity& player,
    EntityIndexType playerIdx,
    EntityIndexType enemyIdx,
    const sf::Vector2f& enemyPosition) const noexcept
{
    const auto directionToEnemy = enemyPosition - player.hitbox.getPosition();
    const auto radialDifference = dgm::Math::getRadialDifference(
        dgm::Math::cartesianToPolar(player.direction).angle,
        dgm::Math::cartesianToPolar(directionToEnemy).angle);
    const bool isWithinConeOfVision = radialDifference < AI_FOV / 2.f;
    if (!isWithinConeOfVision) return false;

    const auto result = hitscanner.hitscan(
        Position { player.hitbox.getPosition() },
        Direction { directionToEnemy },
        playerIdx);

    return result.impactedEntityIdx.has_value()
           && result.impactedEntityIdx.value() == enemyIdx;
}

void AiEngine::rotateTowardTarget(
    const Entity& player,
    mem::Rc<AiController>& input,
    const sf::Vector2f& targetLocation)
{
    const auto dirToTarget = targetLocation - player.hitbox.getPosition();
    const auto pivotDir =
        getVectorPivotDirection(player.direction, dirToTarget);
    input->setSteer(AI_TURN_SPEED_MULTIPLIER * pivotDir);
}

void AiEngine::setPathToTargetLocation(
    const Entity& player,
    const sf::Vector2f& destination,
    AiBlackboard& blackboard)
{
    scene.navmesh.computePath(player.hitbox.getPosition(), destination)
        .and_then(
            [&](auto path) -> std::optional<dgm::Path<dgm::WorldNavpoint>>
            {
#ifdef _DEBUG
                // Not really needed to be logged since I've figured out
                // what is happening, left for possible future diagnostics
                if (path.isTraversed())
                {
                    std::cerr << std::format(
                        "AI Error: Path was found, but was already "
                        "traversed. Going from {} to {}\n",
                        dgm::Utility::to_string(player.hitbox.getPosition()),
                        dgm::Utility::to_string(destination));
                }
#endif

                // Due to leevay to target detection, NPC might slide into
                // destination tile so no path is fouńd, but it needs a little
                // nudge to reach the center especially in CTF
                blackboard.targetLocation = path.isTraversed()
                                                ? destination // a little nudge
                                                : path.getCurrentPoint().coord;
                return path;
            })
        .or_else(
            [&]() -> std::optional<dgm::Path<dgm::WorldNavpoint>>
            {
                std::cerr << std::format(
                    "AI Error: No path was found! Going from {} to {}\n",
                    dgm::Utility::to_string(player.hitbox.getPosition()),
                    dgm::Utility::to_string(destination));
                return std::nullopt;
            });
}
