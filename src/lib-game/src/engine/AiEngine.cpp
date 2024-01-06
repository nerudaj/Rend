#include <core/EntityDefinitions.hpp>
#include <engine/AiEngine.hpp>
#include <utils/MathHelpers.hpp>

import UtilityAi;

#define BIND(f) std::bind(&AiEngine::f, &self, std::placeholders::_1)
#define BIND3(f)                                                               \
    std::bind(                                                                 \
        &AiEngine::f,                                                          \
        &self,                                                                 \
        std::placeholders::_1,                                                 \
        std::placeholders::_2,                                                 \
        std::placeholders::_3)

AiEngine::AiEngine(Scene& scene)
    : scene(scene)
    , hitscanner(scene)
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

dgm::fsm::Fsm<AiTopState, AiBlackboard> AiEngine::createTopFsm(AiEngine& self)
{
    using dgm::fsm::decorator::Not;
    using enum AiTopState;

    auto isThisPlayerDead = Not<AiBlackboard>(BIND(isThisPlayerAlive));

    // clang-format off
    return dgm::fsm::Builder<AiTopState, AiBlackboard>()
        .with(BootstrapAlive)
            .exec([](auto& b) { b.aiState = AiState::ChoosingGatherLocation; }).andGoTo(Alive)
        .with(Alive)
            .when(isThisPlayerDead).goTo(BootstrapDead)
            .otherwiseExec(BIND(runFsmAlive)).andLoop()
        .with(BootstrapDead)
            .exec([](auto& b) { b.aiState = AiState::RequestingRespawn; }).andGoTo(Dead)
        .with(AiTopState::Dead)
            .when(BIND(isThisPlayerAlive)).goTo(BootstrapAlive)
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
    using enum AiState;

    auto gather = Merge<AiBlackboard, Entity, PlayerInventory>(
        BIND3(moveTowardTargetLocation), BIND3(rotateTowardTargetLocation));

    auto canShootTarget = And<AiBlackboard, Entity, PlayerInventory>(
        BIND3(isTargetInReticle), BIND3(canShoot));

    auto doNothing = [](AiBlackboard&, Entity&, PlayerInventory&) {};

    // clang-format off
    return dgm::fsm::Builder<AiState, AiBlackboard, Entity, PlayerInventory>()
        .with(ChoosingGatherLocation)
            .exec(BIND3(pickGatherLocation)).andGoTo(Gathering)
        .with(Gathering)
            .when(BIND3(isTargetLocationReached)).goTo(ChoosingGatherLocation)
            //.orWhen(isEnemyVisible).goTo(LockingTarget)
            // TODO: swap on first and second weapon
            .otherwiseExec(gather).andLoop()
        /*.with(AiState::TryToPickNewTarget)
            .exec(BIND3(pickTargetEnemy)).andGoTo(AiState::Update)
        .with(AiState::ShootTarget)
            .exec(BIND3(shoot)).andGoTo(AiState::Update)
        .with(AiState::SwapWeapon)
            .exec(BIND3(swapWeapon)).andGoTo(AiState::Delay)
        .with(AiState::Delay)
            .exec(doNothing).andGoTo(AiState::Update)*/
        .build();
    // clang-format on
}

dgm::fsm::Fsm<AiState, AiBlackboard> AiEngine::createDeadFsm(AiEngine& self)
{
    using enum AiState;

    auto requestRespawn = [](AiBlackboard& bb) { bb.input->shoot(); };

    // clang-format off
    return dgm::fsm::Builder<AiState, AiBlackboard>()
        .with(RequestingRespawn)
            .exec(requestRespawn)
            .andGoTo(WaitingForRespawn)
        .with(WaitingForRespawn)
            .exec([](auto){})
            .andLoop()
        .build();
    // clang-format on
}

#undef BIND
#undef BIND3

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
}

void AiEngine::runFsmDead(AiBlackboard& blackboard)
{
    fsmDead.setState(blackboard.aiState);
    fsmDead.update(blackboard);
    blackboard.aiState = fsmDead.getState();
}

bool AiEngine::isTrackedEnemyVisible(
    const AiBlackboard& blackboard,
    const Entity& player,
    const PlayerInventory& inventory) const noexcept
{
    if (!isPlayerAlive(blackboard.targetEnemyIdx)) return false;

    const auto& enemy = scene.things[blackboard.targetEnemyIdx];

#ifdef DEBUG_REMOVALS
    std::cout << std::format(
        "AiEngine::isTrackedEnemyVisible(me={}, him={})",
        inventory.ownerIdx,
        blackboard.targetEnemyIdx)
              << std::endl;
#endif
    assert(inventory.ownerIdx != blackboard.targetEnemyIdx);

    return isEnemyVisible(
        inventory.ownerIdx,
        player.hitbox.getPosition(),
        blackboard.targetEnemyIdx,
        enemy.hitbox.getPosition());
}

bool AiEngine::isTargetInReticle(
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

bool AiEngine::isEnemyVisible(
    EntityIndexType myIdx,
    const sf::Vector2f& myPosition,
    EntityIndexType enemyIdx,
    const sf::Vector2f& enemyPosition) const noexcept
{
    return false;
    /*const auto directionToEnemy = enemyPosition - myPosition;
    const auto result = hitscanner.hitscan(
        Position { myPosition }, Direction { directionToEnemy }, myIdx);

    return result.impactedEntityIdx.has_value()
           && result.impactedEntityIdx.value() == enemyIdx;*/
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

void AiEngine::rotateTowardTargetLocation(
    AiBlackboard& blackboard, Entity& player, PlayerInventory&)
{
    const auto dirToTarget =
        blackboard.targetLocation - player.hitbox.getPosition();
    const auto pivotDir =
        getVectorPivotDirection(player.direction, dirToTarget);
    blackboard.input->setSteer(pivotDir);
}

void AiEngine::rotateTowardsEnemy(
    AiBlackboard& blackboard, Entity& player, PlayerInventory&) noexcept
{
    if (!isPlayerAlive(blackboard.targetEnemyIdx)) return;

    const auto enemyPosition =
        scene.things[blackboard.targetEnemyIdx].hitbox.getPosition();
    const auto dirToEnemy = getDirectionToEnemy(
        player.hitbox.getPosition(), blackboard.targetEnemyIdx);

    const auto pivotDir = getVectorPivotDirection(player.direction, dirToEnemy);
    blackboard.input->setSteer(pivotDir);
}

void AiEngine::swapWeapon(
    AiBlackboard& blackboard, Entity&, PlayerInventory&) noexcept
{
    blackboard.input->switchToNextWeapon();
}
