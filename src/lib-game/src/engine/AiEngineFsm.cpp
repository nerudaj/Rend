#include <engine/AiEngine.hpp>

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
        BIND3(isTargetEnemyInReticle), BIND3(canShoot));

    auto doNothing = [](AiBlackboard&, Entity&, PlayerInventory&) {};

    // clang-format off
    return dgm::fsm::Builder<AiState, AiBlackboard, Entity, PlayerInventory>()
        .with(ChoosingGatherLocation)
            .exec(BIND3(pickGatherLocation)).andGoTo(Gathering)
        .with(Gathering)
            .when(BIND3(isTargetLocationReached))
                .goTo(ChoosingGatherLocation)
            .orWhen(BIND3(shouldSwapToLongRangeWeapon))
                .goTo(PickingLongRangeWeaponForSwap)
            .orWhen(BIND3(shouldSwapToShortRangeWeapon))
                .goTo(PickingShortRangeWeaponForSwap)
            //.orWhen(isEnemyVisible).goTo(LockingTarget)
            // TODO: swap on first and second weapon
            .otherwiseExec(gather).andLoop()
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
        .with(ComboSwapping)
            .exec(BIND3(performComboSwap))
                .andGoTo(WaitingForRaiseAnimation)
        .with(WaitingForRaiseAnimation)
            .when(BIND3(hasEnteredWeaponRaise))
                .goTo(ExecutingDelayedTransition)
            .otherwiseExec(doNothing).andLoop()
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