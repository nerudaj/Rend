#pragma once

#include <core/Enums.hpp>
#include <core/Scene.hpp>
#include <core/Types.hpp>
#include <utils/SemanticTypes.hpp>

class SceneBuilder
{
public:
    [[nodiscard]] static Scene buildScene(
        const dgm::ResourceManager& resmgr,
        const sf::Vector2f& baseResolution,
        const Settings& settings);

    [[nodiscard]] static Entity createPlayer(
        const Position& position,
        const Direction& lookDirection,
        PlayerStateIndexType stateId) noexcept;

    [[nodiscard]] static PlayerInventory getDefaultInventory() noexcept;

    [[nodiscard]] static Entity createProjectile(
        EntityType type,
        const Position& position,
        const Direction& direction,
        std::size_t tick) noexcept;

    [[nodiscard]] static Entity createEffect(
        EntityType type, const Position& position, std::size_t frameIdx);

    [[nodiscard]] static Entity
    createPickup(EntityType type, const Position& position);
};