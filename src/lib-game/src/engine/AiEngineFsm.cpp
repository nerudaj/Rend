#include <engine/AiEngine.hpp>
#include <fsm/Builder.hpp>
#include <fsm/exports/MermaidExporter.hpp>
#include <limits>

#define CBND(f) [&self](const AiBlackboard& bb) { return self.f(bb); }
#define BND(f) [&self](AiBlackboard& bb) { self.f(bb); }

AiEngine::AiEngine(Scene& scene, const AiEngineConfig& config)
    : scene(scene)
    , hitscanner(scene)
    , logger("ailog.csv")
    , fsm(createFsm(*this, config.useNullBehavior))
    , preferFlags(config.preferFlags)
{
    if (config.enableLogging) fsm.setLogger(logger);
}

template<class Callable, class BbT>
concept FsmCondition = requires(Callable&& fn, const BbT& bb) {
    {
        fn(bb)
    } -> std::same_as<bool>;
} && fsm::BlackboardTypeConcept<BbT>;

auto Not(FsmCondition<AiBlackboard> auto&& fn)
{
    return [fn = std::move(fn)](const AiBlackboard& bb) { return !fn(bb); };
}

fsm::Fsm<AiBlackboard> AiEngine::createFsm(AiEngine& self, bool useNullBehavior)
{
    auto isThisPlayerDead = Not(CBND(isThisPlayerAlive));
    auto canRequestRespawn = [&self](const AiBlackboard& bb)
    { return self.scene.tick % (bb.playerStateIdx + 1) == 0; };
    auto doNothing = [](AiBlackboard&) {};
    auto bootstrapAlive = [](auto& b)
    {
        b.lastHealth = 100;
        b.targetEnemyIdx = std::numeric_limits<EntityIndexType>::max();
    };

    auto gather = [&self](AiBlackboard& blackboard)
    {
        self.moveTowardTargetLocation(blackboard);
        self.rotateTowardTargetLocation(blackboard);
        self.noteCurrentHealth(blackboard);
    };

    auto gatherWhileHurt = [&self](AiBlackboard& blackboard)
    {
        self.moveTowardTargetLocation(blackboard);
        blackboard.input->setSteer(-1.f * AI_TURN_SPEED_MULTIPLIER);
        self.noteCurrentHealth(blackboard);
    };

    auto setTimer = [](AiBlackboard& bb)
    {
        bb.targettingTimer = bb.personality == AiPersonality::Flash
                                 ? AI_FLASH_REACTION_TIME
                                 : AI_REACTION_TIME;
    };

    auto timerHit = [](const AiBlackboard& bb)
    { return bb.targettingTimer <= 0.f; };

    auto hunt = [&self](AiBlackboard& blackboard)
    {
        self.rotateTowardTargetEnemy(blackboard);
        self.moveInRelationToTargetEnemy(blackboard);
        self.performHuntBookmarking(blackboard);
    };

    auto canShootTarget = [&self](const AiBlackboard& bb)
    { return self.isTargetEnemyInReticle(bb) && self.canShoot(bb); };

    auto noEnemyIsVisible = Not(CBND(isAnyEnemyVisible));

    // clang-format off
    return fsm::Builder<AiBlackboard>()
        .withErrorMachine()
            .useGlobalEntryCondition(isThisPlayerDead)
            .withEntryState("Dead")
                .when(canRequestRespawn).goToState("RequestRespawn")
                .otherwiseExec(doNothing).andLoop()
            .withState("RequestRespawn")
                // respawning is done through fire key
                .exec(BND(shoot)).andGoToState("WaitingForRespawn")
            .withState("WaitingForRespawn")
                .when(CBND(isThisPlayerAlive)).goToState("BootstrapAlive")
                .otherwiseExec(doNothing).andLoop()
            .withState("BootstrapAlive")
                .exec(bootstrapAlive).andRestart()
            .done()
        .withSubmachine("WaitForRaiseAnimation")
            .withEntryState("Wait")
                .when(CBND(hasEnteredWeaponRaise)).finish()
                .otherwiseExec(doNothing).andLoop()
            .done()
        .withSubmachine("PerformComboSwap")
            .withEntryState("Swap")
                .exec(BND(performComboSwap))
                    .andGoToMachine("WaitForRaiseAnimation")
                        .thenFinish()
            .done()
        .withSubmachine("Shoot")
            .withEntryState("Shoot")
                .exec(BND(shoot)).andGoToState("Wait")
            .withState("Wait") // wait for 1 frame (1 tick)
                .exec(doNothing).andFinish()
            .done()
        .withSubmachine("RunToFlagPole")
            .withEntryState("SetDestinationToFlagPole")
                .exec(BND(setDestinationToFlagPole))
                    .andGoToState("Run")
            .withState("StartTimer")
                .exec(setTimer).andGoToState("Run")
            .withState("Run")
                .when(Not(CBND(isThisPlayerFlagCarrier)))
                    .finish()
                .orWhen(CBND(isTargetLocationReached))
                    .goToState("SetDestinationToFlagPole")
                .orWhen(timerHit)
                    .goToState("Shoot")
                .otherwiseExec(gather) // moves toward target location and rotates towards it
                    .andLoop()
            .withState("Shoot")
                .exec(BND(shoot)).andGoToState("StartTimer")
            .done()
        .withSubmachine("SwapToSelectedWeapon")
            .withEntryState("CyclingInventory")
                .when(CBND(isActiveWeaponFlaregunAndLastSomethingElse))
                    .goToMachine("PerformComboSwap")
                        .thenGoToState("CyclingInventory")
                .orWhen(CBND(isTargetWeaponHighlightedInInventory))
                    .goToState("WeaponSwapping")
                .otherwiseExec(BND(selectNextWeapon))
                    .andGoToState("CyclingWaitingForOneFrame")
            .withState("CyclingWaitingForOneFrame")
                .exec(doNothing).andGoToState("CyclingInventory")
            .withState("WeaponSwapping")
                .exec(BND(shoot)) // confirming with fire button
                    .andGoToMachine("WaitForRaiseAnimation")
                        .thenFinish()
            .done()
        .withMainMachine()
            .withEntryState("ChoosingGatherLocation")
                .when([useNullBehavior](const AiBlackboard&) { return useNullBehavior; })
                    .goToState("ChoosingGatherLocation")
                .otherwiseExec(BND(pickGatherLocation)).andGoToState("Gather")
            .withState("Gather")
                .when(CBND(isThisPlayerFlagCarrier))
                    .goToMachine("RunToFlagPole")
                        .thenGoToState("ChoosingGatherLocation")
                .orWhen(CBND(wasHurt)).goToState("GatheringAfterHurt")
                .orWhen(CBND(isTargetLocationReached))
                    .goToState("ChoosingGatherLocation")
                .orWhen(CBND(shouldSwapToLongRangeWeapon))
                    .goToState("PickingLongRangeWeaponForSwap")
                .orWhen(CBND(shouldSwapToShortRangeWeapon))
                    .goToState("PickingShortRangeWeaponForSwap")
                .orWhen(CBND(isAnyEnemyVisible))
                    .goToState("StartTargettingTimer")
                .otherwiseExec(gather).andLoop()
            .withState("GatheringAfterHurt")
                .when(CBND(isTargetLocationReached))
                    .goToState("ChoosingGatherLocation")
                .orWhen(CBND(isAnyEnemyVisible))
                    .goToState("StartTargettingTimer")
                .otherwiseExec(gatherWhileHurt).andLoop()
            .withState("PickingLongRangeWeaponForSwap")
                .exec(BND(pickTargetLongRangeWeapon))
                    .andGoToMachine("SwapToSelectedWeapon")
                        .thenGoToState("ChoosingGatherLocation")
            .withState("PickingShortRangeWeaponForSwap")
                .exec(BND(pickTargetShortRangeWeapon))
                    .andGoToMachine("SwapToSelectedWeapon")
                        .thenGoToState("ChoosingGatherLocation")
            .withState("ComboSwapping")
                .exec(BND(performComboSwap))
                    .andGoToMachine("WaitForRaiseAnimation")
                        .thenGoToState("LockingTarget")
            .withState("StartTargettingTimer")
                .exec(setTimer)
                    .andGoToState("WaitingBeforePickingTargetEnemy")
            .withState("WaitingBeforePickingTargetEnemy")
                 .when(timerHit).goToState("PickingTargetEnemy")
                 .otherwiseExec(doNothing).andLoop()
            .withState("PickingTargetEnemy")
                .when(noEnemyIsVisible)
                    .goToState("ChoosingGatherLocation")
                .otherwiseExec(BND(pickTargetEnemy))
                    .andGoToState("LockingTarget")
            .withState("LockingTarget")
                .when(CBND(isTargetEnemyDead))
                    .goToState("ChoosingGatherLocation")
                .orWhen(CBND(isThisPlayerFlagCarrier))
                    .goToMachine("RunToFlagPole")
                        .thenGoToState("ChoosingGatherLocation")
                .orWhen(CBND(isTargetEnemyOutOfView)).goToState("Pursuing")
                .orWhen(CBND(hasNoAmmoForActiveWeapon))
                    .goToState("ComboSwapping")
                .orWhen(canShootTarget)
                    .goToMachine("Shoot")
                        .thenGoToState("LockingTarget")
                .otherwiseExec(hunt).andLoop()
            .withState("Pursuing")
                .when(CBND(isAnyEnemyVisible)).goToState("PickingTargetEnemy")
                .orWhen(CBND(isTargetLocationReached))
                    .goToState("ChoosingGatherLocation")
                .otherwiseExec(gather).andLoop()
            .done()
        .exportDiagram(fsm::MermaidExporter("./ai.md"))
        .build();
    // clang-format on
}

#undef BND