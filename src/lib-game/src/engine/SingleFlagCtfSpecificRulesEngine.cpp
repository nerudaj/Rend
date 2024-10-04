#include <Configs/Strings.hpp>
#include <builder/SceneBuilder.hpp>
#include <engine/SingleFlagCtfSpecificRulesEngine.hpp>

void SingleFlagCtfSpecificRulesEngine::adjustScore(
    PlayerInventory& inventory, ScoringOccasion occasion)
{
    switch (occasion)
    {
        using enum ScoringOccasion;
    case Suicide:
    case KilledOpponent:
    case KilledTeammate:
        break;
    case FlagDelivered:
        ++inventory.score;
    }
}

GiveResult
SingleFlagCtfSpecificRulesEngine::handleGreyFlagPickup(Entity& player)
{
    swapPlayerSkinToFlagCarrier(player);

    return GiveResult {
        .given = true,
        .removePickup = true,
        .playSound = true,
    };
}

void SingleFlagCtfSpecificRulesEngine::handleDeposit(
    const Entity& player, EntityIndexType playerIdx, const Entity& deposit)
{
    const bool redPlayerDeliveringBlueFlag =
        deposit.typeId == EntityType::BlueFlag
        && player.typeId == EntityType::CarrierRedPlayer;
    const bool bluePlayerDeliveringRedFlag =
        deposit.typeId == EntityType::RedFlag
        && player.typeId == EntityType::CarrierBluePlayer;

    if (redPlayerDeliveringBlueFlag || bluePlayerDeliveringRedFlag)
    {
        eventQueue->emplace<FlagDeliveredGameEvent>(playerIdx);
    }
}

void SingleFlagCtfSpecificRulesEngine::handleFlagDelivered(
    const FlagDeliveredGameEvent& e)
{
    auto& player = scene.things[e.scoringPlayerIdx];
    auto& inventory = scene.playerStates[player.stateIdx].inventory;

    adjustScore(inventory, ScoringOccasion::FlagDelivered);
    swapPlayerSkinBackFromFlagCarrier(player);
    respawnAllGreyFlags();
    triggerFlagScoredSoundForEverybody();
    displayGlobalScoreMessage(player.stateIdx);
}

void SingleFlagCtfSpecificRulesEngine::handlePlayerDied(const Entity& player)
{
    if (player.typeId == EntityType::CarrierRedPlayer
        || player.typeId == EntityType::CarrierBluePlayer)
    {
        scene.things.emplaceBack(SceneBuilder::createPickup(
            EntityType::GreyFlag, Position { player.hitbox.getPosition() }));
    }
}

bool SingleFlagCtfSpecificRulesEngine::isDamageAllowed(
    PlayerStateIndexType originatorStateIdx,
    PlayerStateIndexType receiverStateIdx)
{
    // Prevent friendly fire
    return scene.playerStates[originatorStateIdx].inventory.team
           != scene.playerStates[receiverStateIdx].inventory.team;
}

void SingleFlagCtfSpecificRulesEngine::respawnAllGreyFlags()
{
    for (const auto& flagSpawnPosition : scene.greyFlagSpawns)
    {
        scene.markers.emplaceBack(MarkerItemRespawner {
            .timeout = 5_seconds,
            .pickupType = EntityType::GreyFlag,
            .position = flagSpawnPosition,
        });
    }
}

void SingleFlagCtfSpecificRulesEngine::swapPlayerSkinToFlagCarrier(
    Entity& player)
{
    if (player.typeId == EntityType::RedPlayer)
        player.typeId = EntityType::CarrierRedPlayer;
    else if (player.typeId == EntityType::BluePlayer)
        player.typeId = EntityType::CarrierBluePlayer;
}

void SingleFlagCtfSpecificRulesEngine::swapPlayerSkinBackFromFlagCarrier(
    Entity& player)
{
    if (player.typeId == EntityType::CarrierRedPlayer)
        player.typeId = EntityType::RedPlayer;
    else if (player.typeId == EntityType::CarrierBluePlayer)
        player.typeId = EntityType::BluePlayer;
}

void SingleFlagCtfSpecificRulesEngine::displayGlobalScoreMessage(
    PlayerStateIndexType scoringPlayerStateIdx)
{
    for (auto&& [idx, state] : std::views::enumerate(scene.playerStates))
    {
        state.renderContext.message = HudMessage(std::vformat(
            Strings::Game::XY_SCORED, std::make_format_args(playerNames[idx])));
    }

    scene.playerStates[scoringPlayerStateIdx].renderContext.message =
        HudMessage(Strings::Game::YOU_SCORED);
}

void SingleFlagCtfSpecificRulesEngine::triggerFlagScoredSoundForEverybody()
{
    for (const auto&& stateIdx :
         std::views::iota(0u, scene.playerStates.size()))
    {
        eventQueue->emplace<SoundTriggeredAudioEvent>(
            "flag_score.wav", stateIdx);
    }
}
