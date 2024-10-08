#pragma once

#include <LevelD.hpp>
#include <core/Enums.hpp>
#include <core/Scene.hpp>
#include <core/Types.hpp>
#include <utils/SemanticTypes.hpp>

class SceneBuilder
{
public:
    [[nodiscard]] static Scene
    buildScene(const LevelD& level, size_t maxPlayerCount);

    [[nodiscard]] static Entity createPlayer(
        const Position& position,
        const Direction& lookDirection,
        PlayerStateIndexType stateIdx,
        Team team) noexcept;

    [[nodiscard]] static PlayerInventory getDefaultInventory(
        EntityIndexType ownerIdx, int score, Team team) noexcept;

    [[nodiscard]] static Entity createProjectile(
        EntityType type,
        const Position& position,
        const Direction& direction,
        std::size_t tick,
        PlayerStateIndexType originatorStateIdx) noexcept;

    [[nodiscard]] static Entity createEffect(
        EntityType type, const Position& position, std::size_t frameIdx);

    [[nodiscard]] static Entity
    createPickup(EntityType type, const Position& position);

    [[nodiscard]] static Entity createDecal(
        bool isFlesh, const Position& position, std::size_t tick) noexcept;

    [[nodiscard]] static MarkerDamageOverTime createFlaregunDotMarker(
        PlayerStateIndexType targetStateIdx,
        PlayerStateIndexType originatorStateIdx);
};
