#include <core/EntityDefinitions.hpp>
#include <engine/AiEngine.hpp>
#include <utils/MathHelpers.hpp>

import UtilityAi;

void AiEngine::update(const float deltaTime)
{
    for (auto&& state : scene.playerStates)
    {
        if (!state.blackboard.has_value()) continue;

        auto& blackboard = state.blackboard.value();

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

constexpr bool AiEngine::isTargetEnemyDead(
    const AiBlackboard& blackboard,
    const Entity&,
    const PlayerInventory&) const noexcept
{
    return !isPlayerAlive(blackboard.targetEnemyIdx);
}

bool AiEngine::isTargetEnemyOutOfView(
    const AiBlackboard& blackboard,
    const Entity& player,
    const PlayerInventory& inventory) const noexcept
{
    if (!isPlayerAlive(blackboard.targetEnemyIdx)) return false;

    const auto& enemy = scene.things[blackboard.targetEnemyIdx];

#ifdef DEBUG_REMOVALS
    std::cout << std::format(
        "AiEngine::isTargetEnemyOutOfView(me={}, him={})",
        inventory.ownerIdx,
        blackboard.targetEnemyIdx)
              << std::endl;
#endif
    assert(inventory.ownerIdx != blackboard.targetEnemyIdx);

    return isEnemyVisible(
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
    const AiBlackboard& blackboard,
    const Entity& player,
    const PlayerInventory& inventory) const noexcept
{
    for (auto&& state : scene.playerStates)
    {
        if (state.inventory.ownerIdx == inventory.ownerIdx) continue;

        if (!scene.things.isIndexValid(inventory.ownerIdx)
            || scene.things[inventory.ownerIdx].typeId != EntityType::Player)
            return false;

        return isEnemyVisible(
            player,
            inventory.ownerIdx,
            state.inventory.ownerIdx,
            scene.things[inventory.ownerIdx].hitbox.getPosition());
    }

    return false;
}

void AiEngine::pickGatherLocation(
    AiBlackboard& blackboard, Entity& player, PlayerInventory& inventory)
{
    const auto activeWeaponIdx = weaponTypeToIndex(inventory.activeWeaponType);

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
                                activeWeaponIdx,
                                inventory.acquiredWeapons)
                            / static_cast<float>((distance * distance));

        if (score > bestScore)
        {
            bestScore = score;
            bestPosition = thing.hitbox.getPosition();
        }
    }

    scene.navmesh.computePath(player.hitbox.getPosition(), bestPosition)
        .and_then(
            [&](auto path) -> std::optional<dgm::Path<dgm::WorldNavpoint>>
            {
                if (path.isTraversed()) return path; // should not happen
                blackboard.targetLocation = path.getCurrentPoint().coord;
                return path;
            });
}

void AiEngine::pickTargetEnemy(
    AiBlackboard& blackboard,
    Entity& player,
    PlayerInventory& inventory) noexcept
{ /*
 #ifdef DEBUG_REMOVALS
     std::cout << "AiEngine::pickTargetEnemy()" << std::endl;
 #endif

     blackboard.seekTimeout = SEEK_TIMEOUT;
     blackboard.targetEnemyIdx = std::numeric_limits<EntityIndexType>::max();
     for (PlayerStateIndexType i = 0; i < scene.playerStates.size(); i++)
     {
         auto enemyIdx = scene.playerStates[i].inventory.ownerIdx;
         if (i == blackboard.playerStateIdx || !isPlayerAlive(enemyIdx))
             continue;
         assert(inventory.ownerIdx != enemyIdx);

         const auto& enemy = scene.things[enemyIdx];
         if (isEnemyVisible(
                 inventory.ownerIdx,
                 player.hitbox.getPosition(),
                 enemyIdx,
                 enemy.hitbox.getPosition()))
         {
 #ifdef DEBUG_REMOVALS
             std::cout << "AiEngine::targetSetTo(idx=" << enemyIdx << ")"
                       << std::endl;
 #endif
             blackboard.targetEnemyIdx = enemyIdx;
             return;
         }
     }*/
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
