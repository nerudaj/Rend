#include <engine/AiEngine.hpp>
#include <limits>

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
    fsmTop.setStateToStringHelper(std::map { TOP_STATES_TO_STRING });
    fsmAlive.setStateToStringHelper(std::map { AI_STATE_TO_STRING });
    fsmDead.setStateToStringHelper(std::map { AI_STATE_TO_STRING });

#ifdef DEBUG_REMOVALS
    fsmTop.setLogging(true);
    fsmAlive.setLogging(true);
    fsmDead.setLogging(true);
#endif
}

dgm::fsm::Fsm<AiTopState, AiBlackboard> AiEngine::createTopFsm(AiEngine& self)
{
    using dgm::fsm::decorator::Not;
    using enum AiTopState;

    auto bootstrapAlive = [](auto& b)
    {
        b.aiState = AiState::ChoosingGatherLocation;
        b.lastHealth = 100;
        b.targetEnemyIdx = std::numeric_limits<EntityIndexType>::max();
    };
    auto isThisPlayerDead = Not<AiBlackboard>(BIND(isThisPlayerAlive));

    // clang-format off
    return dgm::fsm::Builder<AiTopState, AiBlackboard>()
        .with(BootstrapAlive)
            .exec(bootstrapAlive).andGoTo(Alive)
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

    auto gather = [&self](
                      AiBlackboard& blackboard,
                      Entity& player,
                      PlayerInventory& inventory)
    {
        self.moveTowardTargetLocation(blackboard, player, inventory);
        self.rotateTowardTargetLocation(blackboard, player, inventory);
        blackboard.lastHealth = player.health;
    };

    auto gatherWhileHurt = [&self](
                               AiBlackboard& blackboard,
                               Entity& player,
                               PlayerInventory& inventory)
    {
        self.moveTowardTargetLocation(blackboard, player, inventory);
        blackboard.input->setSteer(-1.f * AI_TURN_SPEED_MULTIPLIER);
        blackboard.lastHealth = player.health;
    };

    auto hunt = [&self](
                    AiBlackboard& blackboard,
                    Entity& player,
                    PlayerInventory& inventory)
    {
        self.rotateTowardTargetEnemy(blackboard, player, inventory);
        self.moveInRelationToTargetEnemy(blackboard, player, inventory);
        self.performHuntBookmarking(blackboard, player, inventory);
    };

    auto canShootTarget = And<AiBlackboard, Entity, PlayerInventory>(
        BIND3(isTargetEnemyInReticle), BIND3(canShoot));

    auto comboSwappingFlaregun = Merge<AiBlackboard, Entity, PlayerInventory>(
        BIND3(performComboSwap),
        [](AiBlackboard& blackboard, Entity&, PlayerInventory&)
        { blackboard.delayedTransitionState = AiState::CyclingInventory; });

    auto setTimer = [](AiBlackboard& bb, Entity&, PlayerInventory&)
    {
        bb.targettingTimer =
            bb.personality == AiPersonality::Flash ? 0.f : AI_REACTION_TIME;
    };

    auto timerHit =
        [](const AiBlackboard& bb, const Entity&, const PlayerInventory&)
    { return bb.targettingTimer <= 0.f; };

    auto doNothing = [](AiBlackboard&, Entity&, PlayerInventory&) {};

    // clang-format off
    return dgm::fsm::Builder<AiState, AiBlackboard, Entity, PlayerInventory>()
        .with(ChoosingGatherLocation)
            .exec(BIND3(pickGatherLocation)).andGoTo(Gathering)
        .with(Gathering)
            .when(BIND3(wasHurt)).goTo(GatheringAfterHurt)
            .orWhen(BIND3(isTargetLocationReached))
                .goTo(ChoosingGatherLocation)
            .orWhen(BIND3(shouldSwapToLongRangeWeapon))
                .goTo(PickingLongRangeWeaponForSwap)
            .orWhen(BIND3(shouldSwapToShortRangeWeapon))
                .goTo(PickingShortRangeWeaponForSwap)
            .orWhen(BIND3(isAnyEnemyVisible))
                .goTo(StartTargettingTimer)
            .otherwiseExec(gather).andLoop()
        .with(GatheringAfterHurt)
            .when(BIND3(isTargetLocationReached))
                .goTo(ChoosingGatherLocation)
            .orWhen(BIND3(isAnyEnemyVisible))
                .goTo(StartTargettingTimer)
            .otherwiseExec(gatherWhileHurt).andLoop()
        .with(PickingLongRangeWeaponForSwap)
            .exec(BIND3(pickTargetLongRangeWeapon)).andGoTo(CyclingInventory)
        .with(PickingShortRangeWeaponForSwap)
            .exec(BIND3(pickTargetShortRangeWeapon)).andGoTo(CyclingInventory)
        .with(CyclingInventory)
            .when(BIND3(isActiveWeaponFlaregunAndLastSomethingElse))
                .goTo(ComboSwappingBeforeCycling)
            .orWhen(BIND3(isTargetWeaponHighlightedInInventory))
                .goTo(WeaponSwapping)
            .otherwiseExec(BIND3(selectNextWeapon)).andGoTo(CyclingWaitingForOneFrame)
        .with(WeaponSwapping)
            .exec(BIND3(confirmWeaponSelection)).andGoTo(WaitingForRaiseAnimation)
        .with(CyclingWaitingForOneFrame)
            .exec(doNothing).andGoTo(CyclingInventory)
        .with(ComboSwappingBeforeCycling)
            .exec(comboSwappingFlaregun).andGoTo(WaitingForRaiseAnimation)
        .with(ComboSwapping)
            .exec(BIND3(performComboSwap))
                .andGoTo(WaitingForRaiseAnimation)
        .with(StartTargettingTimer)
            .exec(setTimer).andGoTo(WaitingBeforePickingTargetEnemy)
        .with(WaitingBeforePickingTargetEnemy)
             .when(timerHit).goTo(PickingTargetEnemy)
             .otherwiseExec(doNothing).andLoop()
        .with(PickingTargetEnemy)
            .when(Not<AiBlackboard, Entity, PlayerInventory>(BIND3(isAnyEnemyVisible)))
                .goTo(ChoosingGatherLocation)
            .otherwiseExec(BIND3(pickTargetEnemy)).andGoTo(LockingTarget)
        .with(LockingTarget)
            .when(BIND3(isTargetEnemyDead)).goTo(ChoosingGatherLocation)
            .orWhen(BIND3(isTargetEnemyOutOfView)).goTo(Pursuing)
            .orWhen(BIND3(hasNoAmmoForActiveWeapon)).goTo(ComboSwapping)
            .orWhen(canShootTarget).goTo(Shooting)
            .otherwiseExec(hunt).andLoop()
        .with(Pursuing)
            .when(BIND3(isAnyEnemyVisible)).goTo(PickingTargetEnemy)
            .orWhen(BIND3(isTargetLocationReached)).goTo(ChoosingGatherLocation)
            .otherwiseExec(gather).andLoop()
        .with(Shooting)
            .exec(BIND3(shoot)).andGoTo(ShootingWaitingForOneFrame)
        .with(ShootingWaitingForOneFrame)
            .exec(doNothing).andGoTo(LockingTarget)
        .with(WaitingForRaiseAnimation)
            .when(BIND3(hasEnteredWeaponRaise))
                .goTo(ExecutingDelayedTransition)
            .otherwiseExec(doNothing).andLoop()
        .build();
    // clang-format on
}

dgm::fsm::Fsm<AiState, AiBlackboard> AiEngine::createDeadFsm(AiEngine&)
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