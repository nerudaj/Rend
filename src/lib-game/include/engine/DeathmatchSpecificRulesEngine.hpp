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
            .given = false,
            .removePickup = false,
            .playSound = false,
        };
    }

    void
    handleDeposit(const Entity&, EntityIndexType, const Entity&) override {};

    void handleFlagDelivered(const FlagDeliveredGameEvent& e) override {};
};