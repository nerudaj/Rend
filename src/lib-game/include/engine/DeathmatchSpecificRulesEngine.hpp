#pragma once

#include <engine/GameModeSpecificRulesEngineInterface.hpp>

class [[nodiscard]] DeathmatchSpecificRulesEngine final
    : public GameModeSpecificRulesEngineInterface
{
public:
    void
    adjustScore(PlayerInventory& inventory, ScoringOccasion occasion) override
    {
        switch (occasion)
        {
            using enum ScoringOccasion;
        case Suicide:
            --inventory.score;
            break;
        case KilledOpponent:
            ++inventory.score;
            break;
        default:
            break;
        }
    }

    GiveResult handleGreyFlagPickup(Entity&) override
    {
        return GiveResult {
            .given = true,
            .removePickup = true,
            .playSound = false,
        };
    }

    void
    handleDeposit(const Entity&, EntityIndexType, const Entity&) override {};

    void handleFlagDelivered(const FlagDeliveredGameEvent&) override {};

    void handlePlayerDied(const Entity&) override {}

    bool isDamageAllowed(PlayerStateIndexType, PlayerStateIndexType) override
    {
        return true;
    }
};