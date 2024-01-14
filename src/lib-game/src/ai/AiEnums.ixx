module;

#include <map>
#include <string>

export module AiEnums;

export enum class AiTopState { BootstrapAlive, Alive, BootstrapDead, Dead };

export enum class AiState {
    ChoosingGatherLocation,
    Gathering,

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

    PickingTargetEnemy,
    LockingTarget,
    Pursuing,
    Shooting,

    ExecutingDelayedTransition,
};

export const std::map<AiTopState, std::string> TOP_STATES_TO_STRING = {
    { AiTopState::BootstrapAlive, "BootstrapAlive" },
    { AiTopState::Alive, "Alive" },
    { AiTopState::BootstrapDead, "BootstrapDead" },
    { AiTopState::Dead, "Dead" }
};

export const std::map<AiState, std::string> AI_STATE_TO_STRING = {
    { AiState::ChoosingGatherLocation, "ChoosingGatherLocation" },
    { AiState::Gathering, "Gathering" },
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
    { AiState::PickingTargetEnemy, "PickingTargetEnemy" },
    { AiState::LockingTarget, "LockingTarget" },
    { AiState::Pursuing, "Pursuing" },
    { AiState::Shooting, "Shooting" },
    { AiState::ExecutingDelayedTransition, "ExecutingDelayedTransition" },
};
