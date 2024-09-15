#include <ai/UtilityAi.hpp>
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
        fsmTop.setState(blackboard.aiTopState);
        fsmTop.update(blackboard);
        blackboard.aiTopState = fsmTop.getState();
    }
}

void AiEngine::runFsmAlive(AiBlackboard& blackboard)
{
    fsmAlive.setState(blackboard.aiState);
    fsmAlive.update(
        blackboard, getPlayer(blackboard), getInventory(blackboard));
    blackboard.aiState = fsmAlive.getState();
    if (blackboard.aiState == AiState::ExecutingDelayedTransition)
        blackboard.aiState = blackboard.delayedTransitionState;
}

void AiEngine::runFsmDead(AiBlackboard& blackboard)
{
    fsmDead.setState(blackboard.aiState);
    fsmDead.update(blackboard);
    blackboard.aiState = fsmDead.getState();
}

bool AiEngine::isTargetLocationReached(
    const AiBlackboard& blackboard,
    const Entity& player,
    const PlayerInventory&) const noexcept
{
    return dgm::Math::getSize(
               player.hitbox.getPosition() - blackboard.targetLocation)
           < AI_MAX_POSITION_ERROR;
}

bool AiEngine::isTargetEnemyOutOfView(
    const AiBlackboard& blackboard,
    const Entity& player,
    const PlayerInventory& inventory) const noexcept
{
    if (!isPlayerAlive(blackboard.targetEnemyIdx)) return false;

    const auto& enemy = scene.things[blackboard.targetEnemyIdx];

    assert(inventory.ownerIdx != blackboard.targetEnemyIdx);

    return !isEnemyVisible(
        player,
        inventory.ownerIdx,
        blackboard.targetEnemyIdx,
        enemy.hitbox.getPosition());
}

bool AiEngine::isTargetEnemyInReticle(
    const AiBlackboard& blackboard,
    const Entity& player,
    const PlayerInventory&) const noexcept
{
    if (!isPlayerAlive(blackboard.targetEnemyIdx)) return false;

    const auto dirToEnemy = getDirectionToEnemy(
        player.hitbox.getPosition(), blackboard.targetEnemyIdx);

    const float angle =
        std::acos(dgm::Math::getDotProduct(player.direction, dirToEnemy));
    return angle <= AI_MAX_AIM_ERROR;
}

bool AiEngine::isAnyEnemyVisible(
    const AiBlackboard&,
    const Entity& player,
    const PlayerInventory& inventory) const noexcept
{
    return std::any_of(
        scene.playerStates.begin(),
        scene.playerStates.end(),
        [&](const PlayerState& state)
        {
            if (state.inventory.ownerIdx == inventory.ownerIdx
                || !isPlayerAlive(state.inventory.ownerIdx))
                return false;

            return isEnemyVisible(
                player,
                inventory.ownerIdx,
                state.inventory.ownerIdx,
                scene.things[state.inventory.ownerIdx].hitbox.getPosition());
        });
}

bool AiEngine::shouldSwapToLongRangeWeapon(
    const AiBlackboard& blackboard,
    const Entity&,
    const PlayerInventory& inventory) const
{
    return blackboard.personality != AiPersonality::Speartip
           && hasLongRangeWeapon(inventory)
           && !isLongRangeWeaponType(inventory.activeWeaponType)
           && !isLongRangeWeaponType(inventory.lastWeaponType);
}

bool AiEngine::shouldSwapToShortRangeWeapon(
    const AiBlackboard&, const Entity&, const PlayerInventory& inventory) const
{
    return hasShortRangeWeapon(inventory)
           && inventory.activeWeaponType != EntityType::WeaponShotgun
           && inventory.lastWeaponType != EntityType::WeaponShotgun;
}

bool AiEngine::hasNoAmmoForActiveWeapon(
    const AiBlackboard&,
    const Entity&,
    const PlayerInventory& inventory) const noexcept
{
    return inventory.ammo[ammoTypeToAmmoIndex(
               ENTITY_PROPERTIES.at(inventory.activeWeaponType).ammoType)]
           < 3;
}

void AiEngine::pickGatherLocation(
    AiBlackboard& blackboard, Entity& player, PlayerInventory& inventory)
{
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
                                inventory.acquiredWeapons)
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

    scene.navmesh.computePath(player.hitbox.getPosition(), bestPosition)
        .and_then(
            [&](auto path) -> std::optional<dgm::Path<dgm::WorldNavpoint>>
            {
                // Might happen if fallback is too close, never mind, a new one
                // will be picked after few frames
                if (path.isTraversed())
                {
                    std::println(
                        std::cerr,
                        "AI Error: Path was found, but was already traversed.");
                    return path;
                }
                blackboard.targetLocation = path.getCurrentPoint().coord;
                return path;
            })
        .or_else(
            [&]() -> std::optional<dgm::Path<dgm::WorldNavpoint>>
            {
                std::println(
                    std::cerr,
                    "AI Error: No path was found! Going from {} to {}",
                    dgm::Utility::to_string(player.hitbox.getPosition()),
                    dgm::Utility::to_string(bestPosition));
                return std::nullopt;
            });
}

void AiEngine::pickTargetEnemy(
    AiBlackboard& blackboard,
    Entity& player,
    PlayerInventory& inventory) noexcept
{
    for (auto&& state : scene.playerStates)
    {
        if (state.inventory.ownerIdx == inventory.ownerIdx) continue;

        if (!scene.things.isIndexValid(state.inventory.ownerIdx)
            || scene.things[state.inventory.ownerIdx].typeId
                   != EntityType::Player)
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

void AiEngine::moveTowardTargetLocation(
    AiBlackboard& blackboard, Entity& player, PlayerInventory&)
{
    const auto directionToDestination =
        blackboard.targetLocation - player.hitbox.getPosition();
    const float angle = dgm::Math::cartesianToPolar(player.direction).angle;
    const auto lookRelativeThrust = dgm::Math::getRotated(
        dgm::Math::toUnit(directionToDestination), -angle);

    blackboard.input->setThrust(lookRelativeThrust.x);
    blackboard.input->setSidewardThrust(lookRelativeThrust.y);
}

void AiEngine::moveInRelationToTargetEnemy(
    AiBlackboard& blackboard, Entity&, PlayerInventory&)
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
    Entity& player,
    mem::Rc<AiController>& input,
    const sf::Vector2f& targetLocation)
{
    const auto dirToTarget = targetLocation - player.hitbox.getPosition();
    const auto pivotDir =
        getVectorPivotDirection(player.direction, dirToTarget);
    input->setSteer(AI_TURN_SPEED_MULTIPLIER * pivotDir);
}
