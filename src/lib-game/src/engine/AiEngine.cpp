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

    auto requestRespawn = BIND(shoot);

    auto canShootTarget =
        And<AiBlackboard>(BIND(isTargetInReticle), BIND(canShoot));

    // clang-format off
    fsmTop = dgm::fsm::Builder<AiBlackboard, AiTopState>()
        .with(AiTopState::BootstrapAlive)
            .exec(BIND(resetBlackboard))
            .andGoTo(AiTopState::Alive)
        .with(AiTopState::Alive)
            .when(isPlayerDead).goTo(AiTopState::BootstrapDead)
            .otherwiseExec(BIND(runFsmAlive)).andLoop()
        .with(AiTopState::BootstrapDead)
            .exec([](auto& b) { b.aiState = AiState::WaitForRespawnRequest; })
            .andGoTo(AiTopState::Dead)
        .with(AiTopState::Dead)
            .when(Not<AiBlackboard>(isPlayerDead)).goTo(AiTopState::BootstrapAlive)
            .otherwiseExec(BIND(runFsmDead)).andLoop()
        .build();

    fsmDead = dgm::fsm::Builder<AiBlackboard, AiState>()
        .with(AiState::WaitForRespawnRequest)
            .exec(BIND(doNothing)).andGoTo(AiState::RequestRespawn)
        .with(AiState::RequestRespawn)
            .exec(requestRespawn).andGoTo(AiState::WaitForRespawn)
        .with(AiState::WaitForRespawn)
            .exec(BIND(doNothing)).andLoop()
        .build();

    fsmAlive = dgm::fsm::Builder<AiBlackboard, AiState>()
        .with(AiState::Start)
            .exec(BIND(doNothing)).andGoTo(AiState::PickNextJumpPoint)
        .with(AiState::PickNextJumpPoint)
            .exec(BIND(pickJumpPoint)).andGoTo(AiState::Update)
        .with(AiState::Update)
            .when(shouldPickNewTarget).goTo(AiState::TryToPickNewTarget)
            .orWhen(canShootTarget).goTo(AiState::ShootTarget)
            .orWhen(BIND(isJumpPointReached)).goTo(AiState::PickNextJumpPoint)
            .otherwiseExec(updatePositionAndDirection).andLoop()
        .with(AiState::TryToPickNewTarget)
            .exec(BIND(pickTargetEnemy)).andGoTo(AiState::Update)
        .with(AiState::ShootTarget)
            .exec(BIND(shoot)).andGoTo(AiState::Update)
        .build();
    // clang-format on

#undef BIND

#ifdef DEBUG_REMOVALS
    fsmTop.setLogging(true);
    fsmAlive.setLogging(true);
    fsmDead.setLogging(true);
    fsmTop.setStateToStringHelper(std::map { TOP_STATES_TO_STRING });
    fsmAlive.setStateToStringHelper(std::map { AI_STATE_TO_STRING });
    fsmDead.setStateToStringHelper(std::map { AI_STATE_TO_STRING });
#endif
}

void AiEngine::update(const float deltaTime)
{
    for (auto&& state : scene.playerStates)
    {
        if (!state.blackboard.has_value()) continue;

        auto& blackboard = state.blackboard.value();

#ifdef DEBUG_REMOVALS
        std::cout << "AiEngine::update(blackboard.contextIdx = "
                  << blackboard.playerStateIdx << ")" << std::endl;
#endif

        blackboard.seekTimeout =
            std::clamp(blackboard.seekTimeout - deltaTime, 0.f, SEEK_TIMEOUT);

        blackboard.input->clearInputs();
        fsmTop.setState(blackboard.aiTopState);
        fsmTop.update(blackboard);
        blackboard.aiTopState = fsmTop.getState();
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
    // constexpr const unsigned ACTIVE_AMMO_SCORE = 500;
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

void AiEngine::runFsmAlive(AiBlackboard& blackboard)
{
    fsmAlive.setState(blackboard.aiState);
    fsmAlive.update(blackboard);
    blackboard.aiState = fsmAlive.getState();
}

void AiEngine::runFsmDead(AiBlackboard& blackboard)
{
    fsmDead.setState(blackboard.aiState);
    fsmDead.update(blackboard);
    blackboard.aiState = fsmDead.getState();
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
#ifdef DEBUG_REMOVALS
    std::cout << "AiEngine::resetBlackboard()" << std::endl;
#endif

    blackboard.seekTimeout = SEEK_TIMEOUT;
    const auto& inventory = getInventory(blackboard);
    const auto& player = scene.things[inventory.ownerIdx];
    blackboard.trackedEnemyIdx = std::numeric_limits<EntityIndexType>::max();
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
#ifdef DEBUG_REMOVALS
    std::cout << "AiEngine::resetBlackboard()" << std::endl;
#endif
    blackboard.aiState = AiState::Start;
    blackboard.nextStop = sf::Vector2f(0.f, 0.f);
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

#ifdef DEBUG_REMOVALS
    std::cout << std::format(
        "AiEngine::isTrackedEnemyVisible(me={}, him={})",
        inventory.ownerIdx,
        blackboard.trackedEnemyIdx)
              << std::endl;
#endif
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
