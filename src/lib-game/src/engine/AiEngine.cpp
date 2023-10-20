#include <core/EntityTraits.hpp>
#include <engine/AiEngine.hpp>

AiEngine::AiEngine(Scene& scene)
    : scene(scene), hitscanner(scene), navmesh(scene.level.bottomMesh)
{
#define BIND(f) std::bind(&AiEngine::f, this, std::placeholders::_1)

    using dgm::fsm::decorator::And;
    using dgm::fsm::decorator::Merge;
    using dgm::fsm::decorator::Not;

    auto isPlayerDead = Not<AiBlackboard>(BIND(isThisPlayerAlive));

    auto shouldPickNewTarget = And<AiBlackboard>(
        Not<AiBlackboard>(BIND(isTrackedEnemyVisible)),
        BIND(hasSeekTimerElapsed));

    auto updatePositionAndDirection =
        Merge<AiBlackboard>(BIND(moveTowardsTarget), BIND(rotateTowardsEnemy));

    auto requestRespawn =
        Merge<AiBlackboard>(BIND(shoot), BIND(resetBlackboard));

    auto canShootTarget =
        And<AiBlackboard>(BIND(isTargetInReticle), BIND(canShoot));

    // clang-format off
    topFsm = dgm::fsm::Builder<AiBlackboard, AiTopState>()
        .with(AiTopState::Alive)
            .when(isPlayerDead).goTo(AiTopState::Dead)
            .otherwiseExec(BIND(doNothing)).andLoop()
        .with(AiTopState::Dead)
            .when(Not<AiBlackboard>(isPlayerDead)).goTo(AiTopState::Alive)
            .otherwiseExec(BIND(doNothing)).andLoop()
        .build();

    fsm = dgm::fsm::Builder<AiBlackboard, AiState>()
        // Alive
        .with(AiState::Start)
            .when(isPlayerDead).goTo(AiState::WaitForRespawnRequest)
            .otherwiseExec(BIND(doNothing)).andGoTo(AiState::PickNextJumpPoint)
        .with(AiState::PickNextJumpPoint)
            .when(isPlayerDead).goTo(AiState::WaitForRespawnRequest)
            .otherwiseExec(BIND(pickJumpPoint)).andGoTo(AiState::Update)
        .with(AiState::Update)
            .when(isPlayerDead).goTo(AiState::WaitForRespawnRequest)
            .orWhen(shouldPickNewTarget).goTo(AiState::TryToPickNewTarget)
            .orWhen(canShootTarget).goTo(AiState::ShootTarget)
            .orWhen(BIND(isJumpPointReached)).goTo(AiState::PickNextJumpPoint)
            .otherwiseExec(updatePositionAndDirection).andLoop()
        .with(AiState::TryToPickNewTarget)
            .exec(BIND(pickTargetEnemy)).andGoTo(AiState::Update)
        .with(AiState::ShootTarget)
            .exec(BIND(shoot)).andGoTo(AiState::Update)
        // Dead
        .with(AiState::WaitForRespawnRequest)
            .exec(BIND(doNothing)).andGoTo(AiState::RequestRespawn)
        .with(AiState::RequestRespawn)
            .exec(requestRespawn).andGoTo(AiState::WaitForRespawn)
        .with(AiState::WaitForRespawn)
            .when(BIND(isThisPlayerAlive)).goTo(AiState::Start)
            .otherwiseExec(BIND(doNothing)).andLoop()
        .build();
    // clang-format on

#undef BIND

    discoverInterestingLocation();
}

void AiEngine::update(const float deltaTime)
{
    for (auto&& state : scene.playerStates)
    {
        if (!state.blackboard.has_value()) continue;

        auto& blackboard = state.blackboard.value();

        blackboard.seekTimeout =
            std::clamp(blackboard.seekTimeout - deltaTime, 0.f, SEEK_TIMEOUT);

        auto& inventory = getInventory(blackboard);
        blackboard.input->clearInputs();
        fsm.setState(blackboard.aiState);
        fsm.update(blackboard);
        blackboard.aiState = fsm.getState();
    }
}

void AiEngine::discoverInterestingLocation()
{
    // TODO: remove
    for (auto&& [entity, idx] : scene.things)
    {
        if (!isPickable(entity.typeId)) continue;
        if (isWeaponPickable(entity.typeId))
        {
            weaponLocations.emplace_back(
                weaponPickupToIndex(entity.typeId),
                entity.hitbox.getPosition());
        }
        else if (isPowerItemPickable(entity.typeId))
        {
            powerItemLocations.emplace_back(
                entity.typeId, entity.hitbox.getPosition());
        }
        else
        {
            pickupLocations.emplace_back(
                entity.typeId, entity.hitbox.getPosition());
        }
    }
}

int AiEngine::getItemBaseScore(
    EntityType type,
    int myHealth,
    const AcquitedWeaponsArray& acquiredWeapons) const noexcept
{
    constexpr const unsigned MEDIKIT_BASE_SCORE = 100;
    constexpr const unsigned POWERITEM_SCORE = 8000;
    constexpr const unsigned WEAPON_SCORE = 1000;
    // TODO: detect active ammo type
    constexpr const unsigned ACTIVE_AMMO_SCORE = 500;
    constexpr const unsigned ARMOR_SCORE = 100;
    constexpr const unsigned INACTIVE_AMMO_SCORE = 50;

    if (isWeaponPickable(type)
        && !acquiredWeapons[weaponPickupToIndex(type)]) // && unpicked
        return WEAPON_SCORE;
    else if (isPowerItemPickable(type))
        return POWERITEM_SCORE;
    else if (type == EntityType::PickupHealth)
        return (100 - myHealth) * MEDIKIT_BASE_SCORE;
    else if (type == EntityType::PickupArmor)
        return ARMOR_SCORE;
    else if (isPickable(type))
        return INACTIVE_AMMO_SCORE;

    return 0;
}

void AiEngine::pickJumpPoint(AiBlackboard& blackboard)
{
    const auto& inventory = getInventory(blackboard);
    const auto& player = getPlayer(blackboard);

    sf::Vector2f bestPosition;
    float bestScore = 0;
    for (auto&& [thing, idx] : scene.things)
    {
        if (isWeaponPickable(thing.typeId)
            && inventory.acquiredWeapons[weaponPickupToIndex(thing.typeId)])
            continue;

        int currentScore = getItemBaseScore(
            thing.typeId, player.health, inventory.acquiredWeapons);
        float distance = dgm::Math::getSize(
            thing.hitbox.getPosition() - player.hitbox.getPosition());
        float score = currentScore / (distance * distance);

        if (score > bestScore)
        {
            bestScore = score;
            bestPosition = thing.hitbox.getPosition();
        }
    }

    const auto& path =
        navmesh.getPath(player.hitbox.getPosition(), bestPosition);
    if (path.isTraversed()) return;
    blackboard.nextStop = path.getCurrentPoint().coord;
}

void AiEngine::pickTargetEnemy(AiBlackboard& blackboard) noexcept
{
    blackboard.seekTimeout = SEEK_TIMEOUT;
    const auto& inventory = getInventory(blackboard);
    const auto& player = scene.things[inventory.ownerIdx];
    for (PlayerStateIndexType i = 0; i < scene.playerStates.size(); i++)
    {
        auto enemyIdx = scene.playerStates[i].inventory.ownerIdx;
        if (i == blackboard.playerStateIdx || !isPlayerAlive(enemyIdx))
            continue;

        const auto& enemy = scene.things[enemyIdx];
        if (isEnemyVisible(
                inventory.ownerIdx,
                player.hitbox.getPosition(),
                enemyIdx,
                enemy.hitbox.getPosition()))
        {
            blackboard.trackedEnemyIdx = enemyIdx;
            return;
        }
    }
}

bool AiEngine::isEnemyVisible(
    EntityIndexType myIdx,
    const sf::Vector2f& myPosition,
    EntityIndexType enemyIdx,
    const sf::Vector2f& enemyPosition) const noexcept
{
    const auto directionToEnemy = enemyPosition - myPosition;
    const auto result = hitscanner.hitscan(
        Position { myPosition }, Direction { directionToEnemy }, myIdx);

    return result.impactedEntityIdx.has_value()
           && result.impactedEntityIdx.value() == enemyIdx;
}

bool AiEngine::isJumpPointReached(const AiBlackboard& blackboard) const noexcept
{
    const auto& player = getPlayer(blackboard);
    return dgm::Math::getSize(player.hitbox.getPosition() - blackboard.nextStop)
           < 2.f;
}

void AiEngine::moveTowardsTarget(AiBlackboard& blackboard)
{
    const auto& player = getPlayer(blackboard);
    const auto directionToDestination =
        blackboard.nextStop - player.hitbox.getPosition();
    const float angle = dgm::Math::cartesianToPolar(player.direction).angle;
    const auto lookRelativeThrust = dgm::Math::getRotated(
        dgm::Math::toUnit(directionToDestination), -angle);

    blackboard.input->setThrust(lookRelativeThrust.x);
    blackboard.input->setSidewardThrust(lookRelativeThrust.y);
}

void AiEngine::resetBlackboard(AiBlackboard& blackboard) const noexcept
{
    blackboard.nextStop = sf::Vector2f(0.f, 0.f);
    blackboard.targetLocation = sf::Vector2f(0.f, 0.f);
    blackboard.trackedEnemyIdx = std::numeric_limits<EntityIndexType>::max();
}

void AiEngine::rotateTowardsEnemy(AiBlackboard& blackboard) noexcept
{
    if (!isPlayerAlive(blackboard.trackedEnemyIdx)) return;

    const auto& player = getPlayer(blackboard);
    const auto enemyPosition =
        scene.things[blackboard.trackedEnemyIdx].hitbox.getPosition();
    const auto dirToEnemy = getDirectionToEnemy(
        player.hitbox.getPosition(), blackboard.trackedEnemyIdx);

    const auto pivotDir = getVectorPivotDirection(player.direction, dirToEnemy);
    blackboard.input->setSteer(pivotDir);
}

bool AiEngine::isTrackedEnemyVisible(
    const AiBlackboard& blackboard) const noexcept
{
    if (!isPlayerAlive(blackboard.trackedEnemyIdx)) return false;

    const auto& enemy = scene.things[blackboard.trackedEnemyIdx];
    const auto& inventory = getInventory(blackboard);
    const auto& player = scene.things[inventory.ownerIdx];
    assert(inventory.ownerIdx != blackboard.trackedEnemyIdx);

    return isEnemyVisible(
        inventory.ownerIdx,
        player.hitbox.getPosition(),
        blackboard.trackedEnemyIdx,
        enemy.hitbox.getPosition());
}

bool AiEngine::isTargetInReticle(const AiBlackboard& blackboard) const noexcept
{
    if (!isPlayerAlive(blackboard.trackedEnemyIdx)) return false;

    const auto& player = getPlayer(blackboard);
    const auto dirToEnemy = getDirectionToEnemy(
        player.hitbox.getPosition(), blackboard.trackedEnemyIdx);

    const float angle =
        std::acos(dgm::Math::getDotProduct(player.direction, dirToEnemy));
    return angle <= std::numbers::pi_v<float> / 16;
}
