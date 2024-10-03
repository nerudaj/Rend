#pragma once

#include <core/Scene.hpp>
#include <enums/ScoringOccasion.hpp>
#include <events/GameRuleEvents.hpp>
#include <utils/GiveResult.hpp>

class [[nodiscard]] GameModeSpecificRulesEngineInterface
{
public:
    virtual ~GameModeSpecificRulesEngineInterface() = default;

public:
    virtual void
    adjustScore(PlayerInventory& inventory, ScoringOccasion occasion) = 0;

    virtual GiveResult handleGreyFlagPickup(Entity& player) = 0;

    virtual void handleDeposit(
        const Entity& player,
        EntityIndexType playerIdx,
        const Entity& deposit) = 0;

    virtual void handleFlagDelivered(const FlagDeliveredGameEvent& e) = 0;

    virtual void handlePlayerDied(const Entity& player) = 0;

    virtual bool isDamageAllowed(
        PlayerStateIndexType originatorStateIdx,
        PlayerStateIndexType receiverStateIdx) = 0;
};
