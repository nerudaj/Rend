#include <core/EntityTraits.hpp>
#include <engine/AiEngine.hpp>

AiEngine::AiEngine(Scene& scene)
    : scene(scene)
    , hitscanner(scene)
    , navmesh(scene.level.bottomMesh)
    , fsmTop(createTopFsm(*this))
    , fsmAlive(createAliveFsm(*this))
    , fsmDead(createDeadFsm(*this))
{

#ifdef DEBUG_REMOVALS
    fsmTop.setLogging(true);
    fsmAlive.setLogging(true);
    fsmDead.setLogging(true);
    fsmTop.setStateToStringHelper(std::map { TOP_STATES_TO_STRING });
    fsmAlive.setStateToStringHelper(std::map { AI_STATE_TO_STRING });
    fsmDead.setStateToStringHelper(std::map { AI_STATE_TO_STRING });
#endif
}

#define BIND(f) std::bind(&AiEngine::f, &self, std::placeholders::_1)
#define BIND3(f)                                                               \
    std::bind(                                                                 \
        &AiEngine::f,                                                          \
        &self,                                                                 \
        std::placeholders::_1,                                                 \
        std::placeholders::_2,                                                 \
        std::placeholders::_3)

dgm::fsm::Fsm<AiTopState, AiBlackboard> AiEngine::createTopFsm(AiEngine& self)
{
    using dgm::fsm::decorator::Not;

    auto isPlayerDead = Not<AiBlackboard>(BIND(isThisPlayerAlive));

    // clang-format off
    return dgm::fsm::Builder<AiTopState, AiBlackboard>()
        .with(AiTopState::BootstrapAlive)
            .exec(BIND(resetBlackboard)).andGoTo(AiTopState::Alive)
        .with(AiTopState::Alive)
            .when(isPlayerDead).goTo(AiTopState::BootstrapDead)
            .otherwiseExec(BIND(runFsmAlive)).andLoop()
        .with(AiTopState::BootstrapDead)
            .exec([](auto& b) { b.aiState = AiState::WaitForRespawnRequest; }).andGoTo(AiTopState::Dead)
        .with(AiTopState::Dead)
            .when(Not<AiBlackboard>(isPlayerDead)).goTo(AiTopState::BootstrapAlive)
            .otherwiseExec(BIND(runFsmDead)).andLoop()
        .build();
    // clang-format on
}

dgm::fsm::Fsm<AiState, AiBlackboard, Entity, PlayerInventory>
AiEngine::createAliveFsm(AiEngine& self)
{
    using dgm::fsm::decorator::And;
    using dgm::fsm::decorator::Merge;
    using dgm::fsm::decorator::Not;

    auto shouldPickNewTarget = And<AiBlackboard, Entity, PlayerInventory>(
        Not<AiBlackboard, Entity, PlayerInventory>(
            BIND3(isTrackedEnemyVisible)),
        BIND3(hasSeekTimerElapsed));

    auto updatePositionAndDirection =
        Merge<AiBlackboard, Entity, PlayerInventory>(
            BIND3(moveTowardsTarget), BIND3(rotateTowardsEnemy));

    auto canShootTarget = And<AiBlackboard, Entity, PlayerInventory>(
        BIND3(isTargetInReticle), BIND3(canShoot));

    auto doNothing = [](AiBlackboard&, Entity&, PlayerInventory&) {};

    // clang-format off
    return dgm::fsm::Builder<AiState, AiBlackboard, Entity, PlayerInventory>()
        .with(AiState::Start)
            .exec(doNothing).andGoTo(AiState::PickNextJumpPoint)
        .with(AiState::PickNextJumpPoint)
            .exec(BIND3(pickJumpPoint)).andGoTo(AiState::Update)
        .with(AiState::Update)
            .when(shouldPickNewTarget).goTo(AiState::TryToPickNewTarget)
            .orWhen(canShootTarget).goTo(AiState::ShootTarget)
            .orWhen(BIND3(isJumpPointReached)).goTo(AiState::PickNextJumpPoint)
            .orWhen(BIND3(shouldSwapWeapon)).goTo(AiState::SwapWeapon)
            .otherwiseExec(updatePositionAndDirection).andLoop()
        .with(AiState::TryToPickNewTarget)
            .exec(BIND3(pickTargetEnemy)).andGoTo(AiState::Update)
        .with(AiState::ShootTarget)
            .exec(BIND3(shoot)).andGoTo(AiState::Update)
        .with(AiState::SwapWeapon)
            .exec(BIND3(swapWeapon)).andGoTo(AiState::Delay)
        .with(AiState::Delay)
            .exec(doNothing).andGoTo(AiState::Update)
        .build();
    // clang-format on
}

dgm::fsm::Fsm<AiState, AiBlackboard> AiEngine::createDeadFsm(AiEngine& self)
{
    auto requestRespawn = [](AiBlackboard& bb) { bb.input->shoot(); };

    // clang-format off
    return dgm::fsm::Builder<AiState, AiBlackboard>()
        .with(AiState::WaitForRespawnRequest)
        .exec(BIND(doNothing))
        .andGoTo(AiState::RequestRespawn)
        .with(AiState::RequestRespawn)
        .exec(requestRespawn)
        .andGoTo(AiState::WaitForRespawn)
        .with(AiState::WaitForRespawn)
        .exec(BIND(doNothing))
        .andLoop()
        .build();
    // clang-format on
}

#undef BIND

void AiEngine::update(const float deltaTime)
{
    for (auto&& state : scene.playerStates)
    {
        if (!state.blackboard.has_value()) continue;

        auto& blackboard = state.blackboard.value();

#ifdef DEBUG_REMOVALS
        fsmTop.setLogging(blackboard.playerStateIdx == 0);
        fsmAlive.setLogging(blackboard.playerStateIdx == 0);
        fsmDead.setLogging(blackboard.playerStateIdx == 0);
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
    const PlayerInventory& inventory) const noexcept
{
    constexpr const unsigned MEDIKIT_BASE_SCORE = 100;
    constexpr const unsigned POWERITEM_SCORE = 8000;
    constexpr const unsigned WEAPON_SCORE = 1000;
    constexpr const unsigned ARMOR_SCORE = 100;
    constexpr const unsigned AMMO_SCORE = 50;

    if (isWeaponPickable(type)
        && !inventory.acquiredWeapons[weaponPickupToIndex(type)]) // && unpicked
        return WEAPON_SCORE;
    else if (isPowerItemPickable(type))
        return POWERITEM_SCORE;
    else if (type == EntityType::PickupHealth)
        return (100 - myHealth) * MEDIKIT_BASE_SCORE;
    else if (type == EntityType::PickupArmor)
        return ARMOR_SCORE;
    else if (isPickable(type))
    {
        if ((type == EntityType::PickupBullets
             && inventory.bulletCount < MAX_BULLETS)
            || (type == EntityType::PickupShells
                && inventory.shellCount < MAX_SHELLS)
            || (type == EntityType::PickupEnergy
                && inventory.energyCount < MAX_ENERGY)
            || (type == EntityType::PickupRockets
                && inventory.rocketCount < MAX_ROCKETS))
            return AMMO_SCORE;
        return -1;
    }

    return 0;
}

void AiEngine::runFsmAlive(AiBlackboard& blackboard)
{
    auto& inventory = getInventory(blackboard);
    auto& player = getPlayer(blackboard);
    fsmAlive.setState(blackboard.aiState);
    fsmAlive.update(blackboard, player, inventory);
    blackboard.aiState = fsmAlive.getState();
}

void AiEngine::runFsmDead(AiBlackboard& blackboard)
{
    fsmDead.setState(blackboard.aiState);
    fsmDead.update(blackboard);
    blackboard.aiState = fsmDead.getState();
}

bool AiEngine::shouldSwapWeapon(
    const AiBlackboard& blackboard,
    const Entity&,
    const PlayerInventory& inventory) const noexcept
{
    auto index = blackboard.playerStateIdx;
    return inventory.acquiredWeapons[index]
           && weaponTypeToIndex(inventory.activeWeaponType) != index;
}

void AiEngine::pickJumpPoint(
    AiBlackboard& blackboard, Entity& player, PlayerInventory& inventory)
{
    auto&& bestPosition = sf::Vector2f(0.f, 0.f);
    float bestScore = 0;
    for (auto&& [thing, idx] : scene.things)
    {
        if (isWeaponPickable(thing.typeId)
            && inventory.acquiredWeapons[weaponPickupToIndex(thing.typeId)])
            continue;

        int currentScore =
            getItemBaseScore(thing.typeId, player.health, inventory);
        float distance = dgm::Math::getSize(
            thing.hitbox.getPosition() - player.hitbox.getPosition());
        float score = currentScore / (distance * distance);

        if (score > bestScore)
        {
            bestScore = score;
            bestPosition = thing.hitbox.getPosition();
        }
    }

    // TODO: this
    // assert(bestPosition.x != 0.f && bestPosition.y != 0.f);
    if (bestPosition.x == 0.f && bestPosition.y == 0.f) return;

    const auto& path =
        navmesh.getPath(player.hitbox.getPosition(), bestPosition);
    if (path.isTraversed()) return;
    blackboard.nextStop = path.getCurrentPoint().coord;
}

void AiEngine::pickTargetEnemy(
    AiBlackboard& blackboard,
    Entity& player,
    PlayerInventory& inventory) noexcept
{
#ifdef DEBUG_REMOVALS
    std::cout << "AiEngine::pickTargetEnemy()" << std::endl;
#endif

    blackboard.seekTimeout = SEEK_TIMEOUT;
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

bool AiEngine::isJumpPointReached(
    const AiBlackboard& blackboard,
    const Entity& player,
    const PlayerInventory& inventory) const noexcept
{
    return dgm::Math::getSize(player.hitbox.getPosition() - blackboard.nextStop)
           < AI_MAX_POSITION_ERROR;
}

void AiEngine::moveTowardsTarget(
    AiBlackboard& blackboard, Entity& player, PlayerInventory&)
{
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

void AiEngine::rotateTowardsEnemy(
    AiBlackboard& blackboard, Entity& player, PlayerInventory&) noexcept
{
    if (!isPlayerAlive(blackboard.trackedEnemyIdx)) return;

    const auto enemyPosition =
        scene.things[blackboard.trackedEnemyIdx].hitbox.getPosition();
    const auto dirToEnemy = getDirectionToEnemy(
        player.hitbox.getPosition(), blackboard.trackedEnemyIdx);

    const auto pivotDir = getVectorPivotDirection(player.direction, dirToEnemy);
    blackboard.input->setSteer(pivotDir);
}

void AiEngine::swapWeapon(
    AiBlackboard& blackboard, Entity&, PlayerInventory&) noexcept
{
    blackboard.input->switchToNextWeapon();
}

bool AiEngine::isTrackedEnemyVisible(
    const AiBlackboard& blackboard,
    const Entity& player,
    const PlayerInventory& inventory) const noexcept
{
    if (!isPlayerAlive(blackboard.trackedEnemyIdx)) return false;

    const auto& enemy = scene.things[blackboard.trackedEnemyIdx];

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

bool AiEngine::isTargetInReticle(
    const AiBlackboard& blackboard,
    const Entity& player,
    const PlayerInventory&) const noexcept
{
    if (!isPlayerAlive(blackboard.trackedEnemyIdx)) return false;

    const auto dirToEnemy = getDirectionToEnemy(
        player.hitbox.getPosition(), blackboard.trackedEnemyIdx);

    const float angle =
        std::acos(dgm::Math::getDotProduct(player.direction, dirToEnemy));
    return angle <= AI_MAX_AIM_ERROR;
}
