#include <engine/AiEngine.hpp>
#include <fsm/Builder.hpp>
#include <fsm/exports/MermaidExporter.hpp>
#include <limits>

#define BIND(f) std::bind(&AiEngine::f, &self, std::placeholders::_1)

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
    auto isThisPlayerDead = Not(BIND(isThisPlayerAlive));
    auto shouldRequestRespawn = [&self](const AiBlackboard& bb)
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
        blackboard.lastHealth = self.getPlayer(blackboard).health;
    };

    auto gatherWhileHurt = [&self](AiBlackboard& blackboard)
    {
        self.moveTowardTargetLocation(blackboard);
        blackboard.input->setSteer(-1.f * AI_TURN_SPEED_MULTIPLIER);
        blackboard.lastHealth = self.getPlayer(blackboard).health;
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

    auto noEnemyIsVisible = Not(BIND(isAnyEnemyVisible));

    // clang-format off
    return fsm::Builder<AiBlackboard>()
        .withErrorMachine()
            .useGlobalEntryCondition(isThisPlayerDead)
            .withEntryState("Dead")
                .when(shouldRequestRespawn).goToState("RequestRespawn")
                .otherwiseExec(doNothing).andLoop()
            .withState("RequestRespawn")
                // respawning is done through fire key
                .exec(BIND(shoot)).andGoToState("WaitingForRespawn")
            .withState("WaitingForRespawn")
                .when(BIND(isThisPlayerAlive)).goToState("BootstrapAlive")
                .otherwiseExec(doNothing).andLoop()
            .withState("BootstrapAlive")
                .exec(bootstrapAlive).andRestart()
            .done()
        .withSubmachine("WaitForRaiseAnimation")
            .withEntryState("Wait")
                .when(BIND(hasEnteredWeaponRaise)).finish()
                .otherwiseExec(doNothing).andLoop()
            .done()
        .withSubmachine("PerformComboSwap")
            .withEntryState("Swap")
                .exec(BIND(performComboSwap))
                    .andGoToMachine("WaitForRaiseAnimation")
                        .thenFinish()
            .done()
        .withSubmachine("Shoot")
            .withEntryState("Shoot")
                .exec(BIND(shoot)).andGoToState("Wait")
            .withState("Wait") // wait for 1 frame (1 tick)
                .exec(doNothing).andFinish()
            .done()
        .withSubmachine("RunToFlagPole")
            .withEntryState("SetDestinationToFlagPole")
                .exec(BIND(setDestinationToFlagPole))
                    .andGoToState("Run")
            .withState("StartTimer")
                .exec(setTimer).andGoToState("Run")
            .withState("Run")
                .when(Not(BIND(isThisPlayerFlagCarrier)))
                    .finish()
                .orWhen(BIND(isTargetLocationReached))
                    .goToState("SetDestinationToFlagPole")
                .orWhen(timerHit)
                    .goToState("Shoot")
                .otherwiseExec(gather) // moves toward target location and rotates towards it
                    .andLoop()
            .withState("Shoot")
                .exec(BIND(shoot)).andGoToState("StartTimer")
            .done()
        .withMainMachine()
            .withEntryState("ChoosingGatherLocation")
                .when([useNullBehavior](const AiBlackboard&) { return useNullBehavior; })
                    .goToState("ChoosingGatherLocation")
                .otherwiseExec(BIND(pickGatherLocation)).andGoToState("Gather")
            .withState("Gather")
                .when(BIND(isThisPlayerFlagCarrier))
                    .goToMachine("RunToFlagPole")
                        .thenGoToState("ChoosingGatherLocation")
                .orWhen(BIND(wasHurt)).goToState("GatheringAfterHurt")
                .orWhen(BIND(isTargetLocationReached))
                    .goToState("ChoosingGatherLocation")
                .orWhen(BIND(shouldSwapToLongRangeWeapon))
                    .goToState("PickingLongRangeWeaponForSwap")
                .orWhen(BIND(shouldSwapToShortRangeWeapon))
                    .goToState("PickingShortRangeWeaponForSwap")
                .orWhen(BIND(isAnyEnemyVisible))
                    .goToState("StartTargettingTimer")
                .otherwiseExec(gather).andLoop()
            .withState("GatheringAfterHurt")
                .when(BIND(isTargetLocationReached))
                    .goToState("ChoosingGatherLocation")
                .orWhen(BIND(isAnyEnemyVisible))
                    .goToState("StartTargettingTimer")
                .otherwiseExec(gatherWhileHurt).andLoop()
            .withState("PickingLongRangeWeaponForSwap")
                .exec(BIND(pickTargetLongRangeWeapon))
                    .andGoToState("CyclingInventory")
            .withState("PickingShortRangeWeaponForSwap")
                .exec(BIND(pickTargetShortRangeWeapon))
                    .andGoToState("CyclingInventory")
            .withState("CyclingInventory")
                .when(BIND(isActiveWeaponFlaregunAndLastSomethingElse))
                    .goToMachine("PerformComboSwap")
                        .thenGoToState("CyclingInventory")
                .orWhen(BIND(isTargetWeaponHighlightedInInventory))
                    .goToState("WeaponSwapping")
                .otherwiseExec(BIND(selectNextWeapon))
                    .andGoToState("CyclingWaitingForOneFrame")
            .withState("WeaponSwapping")
                .exec(BIND(shoot)) // confirming with fire button
                    .andGoToMachine("WaitForRaiseAnimation")
                        .thenGoToState("ChoosingGatherLocation")
            .withState("CyclingWaitingForOneFrame")
                .exec(doNothing).andGoToState("CyclingInventory")
            .withState("ComboSwapping")
                .exec(BIND(performComboSwap))
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
                .otherwiseExec(BIND(pickTargetEnemy))
                    .andGoToState("LockingTarget")
            .withState("LockingTarget")
                .when(BIND(isTargetEnemyDead))
                    .goToState("ChoosingGatherLocation")
                .orWhen(BIND(isThisPlayerFlagCarrier))
                    .goToMachine("RunToFlagPole")
                        .thenGoToState("ChoosingGatherLocation")
                .orWhen(BIND(isTargetEnemyOutOfView)).goToState("Pursuing")
                .orWhen(BIND(hasNoAmmoForActiveWeapon))
                    .goToState("ComboSwapping")
                .orWhen(canShootTarget)
                    .goToMachine("Shoot")
                        .thenGoToState("LockingTarget")
                .otherwiseExec(hunt).andLoop()
            .withState("Pursuing")
                .when(BIND(isAnyEnemyVisible)).goToState("PickingTargetEnemy")
                .orWhen(BIND(isTargetLocationReached))
                    .goToState("ChoosingGatherLocation")
                .otherwiseExec(gather).andLoop()
            .done()
        .exportDiagram(fsm::MermaidExporter("./ai.md"))
        .build();
    // clang-format on
}

#undef BIND