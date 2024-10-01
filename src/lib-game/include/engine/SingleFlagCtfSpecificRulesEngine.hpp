#pragma once

#include <engine/GameModeSpecificRulesEngineInterface.hpp>
#include <events/EventQueue.hpp>

class [[nodiscard]] SingleFlagCtfSpecificRulesEngine final
    : public GameModeSpecificRulesEngineInterface
{
public:
    SingleFlagCtfSpecificRulesEngine(
        Scene& scene,
        mem::Rc<EventQueue> eventQueue,
        const std::vector<std::string>& playerNames) noexcept
        : scene(scene), eventQueue(eventQueue), playerNames(playerNames)
    {
    }

public:
    void
    adjustScore(PlayerInventory& inventory, ScoringOccasion occasion) override;

    GiveResult handleGreyFlagPickup(Entity&) override;

    void handleDeposit(
        const Entity& player,
        EntityIndexType playerIdx,
        const Entity& deposit) override;

    void handleFlagDelivered(const FlagDeliveredGameEvent& e) override;

    void handlePlayerDied(const Entity& player) override;

private:
    void respawnAllGreyFlags();

    void swapPlayerSkinToFlagCarrier(Entity& player);

    void swapPlayerSkinBackFromFlagCarrier(Entity& player);

    void displayGlobalScoreMessage(PlayerStateIndexType scoringPlayerStateIdx);

private:
    Scene& scene;
    mem::Rc<EventQueue> eventQueue;
    std::vector<std::string> playerNames;
};