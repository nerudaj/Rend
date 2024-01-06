module;

#include <map>
#include <string>

export module AiEnums;

export enum class AiTopState { BootstrapAlive, Alive, BootstrapDead, Dead };

export enum class AiState {
    ChoosingGatherLocation,
    Gathering,
    LockingTarget,
    Dueling,
    Pursuing,
    Shooting,

    RequestingRespawn,
    WaitingForRespawn
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
    { AiState::Dueling, "Dueling" },
    { AiState::Pursuing, "Pursuing" },
    { AiState::Shooting, "Shooting" },
    { AiState::RequestingRespawn, "RequestingRespawn" },
    { AiState::WaitingForRespawn, "WaitingForRespawn" },
};
