#pragma once

#include <map>
#include <string>

enum class [[nodiscard]] AiTopState
{
    BootstrapAlive,
    Alive,
    BootstrapDead,
    Dead
};

enum class [[nodiscard]] AiState
{
    ChoosingGatherLocation,
    Gathering,
    GatheringAfterHurt,

    RequestingRespawn,
    WaitingForRespawn,

    PickingShortRangeWeaponForSwap,
    PickingLongRangeWeaponForSwap,

    ComboSwappingBeforeCycling,
    CyclingInventory,
    CyclingWaitingForOneFrame,
    WeaponSwapping,
    WaitingForRaiseAnimation,

    ComboSwapping,

    StartTargettingTimer,
    WaitingBeforePickingTargetEnemy,
    PickingTargetEnemy,
    LockingTarget,
    Pursuing,
    Shooting,
    ShootingWaitingForOneFrame,

    ExecutingDelayedTransition,
};

const std::map<AiTopState, std::string> TOP_STATES_TO_STRING = {
    { AiTopState::BootstrapAlive, "BootstrapAlive" },
    { AiTopState::Alive, "Alive" },
    { AiTopState::BootstrapDead, "BootstrapDead" },
    { AiTopState::Dead, "Dead" }
};

const std::map<AiState, std::string> AI_STATE_TO_STRING = {
    { AiState::ChoosingGatherLocation, "ChoosingGatherLocation" },
    { AiState::Gathering, "Gathering" },
    { AiState::GatheringAfterHurt, "GatheringAfterHurt" },
    { AiState::RequestingRespawn, "RequestingRespawn" },
    { AiState::WaitingForRespawn, "WaitingForRespawn" },
    { AiState::PickingShortRangeWeaponForSwap,
      "PickingShortRangeWeaponForSwap" },
    { AiState::PickingLongRangeWeaponForSwap, "PickingLongRangeWeaponForSwap" },
    { AiState::ComboSwappingBeforeCycling, "ComboSwappingBeforeCycling" },
    { AiState::CyclingInventory, "CyclingInventory" },
    { AiState::CyclingWaitingForOneFrame, "CyclingWaitingForOneFrame" },
    { AiState::WeaponSwapping, "WeaponSwapping" },
    { AiState::WaitingForRaiseAnimation, "WaitingForRaiseAnimation" },
    { AiState::ComboSwapping, "ComboSwapping" },
    { AiState::WaitingBeforePickingTargetEnemy,
      "WaitingBeforePickingTargetEnemy" },
    { AiState::PickingTargetEnemy, "PickingTargetEnemy" },
    { AiState::LockingTarget, "LockingTarget" },
    { AiState::Pursuing, "Pursuing" },
    { AiState::Shooting, "Shooting" },
    { AiState::ShootingWaitingForOneFrame, "ShootingWaitingForOneFrame" },
    { AiState::ExecutingDelayedTransition, "ExecutingDelayedTransition" },
};

enum class [[nodiscard]] AiPersonality
{
    Default,
    Tank,     // ignores hurt
    Flash,    // no targetting timer,
    Speartip, // rushes with shotgun
};
