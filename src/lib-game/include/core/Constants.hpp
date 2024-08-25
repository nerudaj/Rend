#pragma once

#include <Literals.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <core/Enums.hpp>
#include <core/Types.hpp>
#include <map>
#include <numbers>
#include <utils/SemanticTypes.hpp>

// Movement
constinit const float PLAYER_RADIAL_SPEED = 88.f; // degrees per second
constinit const float PROJECTILE_FORWARD_SPEED = 108.f;

// Game logic
constinit const unsigned FPS = 60;
constinit const float FRAME_TIME = 1.f / FPS;
constinit const float WEAPON_SELECTION_TIMEOUT = 2_seconds;
constinit const int PLAYER_INITIAL_HEALTH = 100;
constinit const int PLAYER_INITIAL_ARMOR = 0;
constinit const int PLAYER_INITIAL_ROCKETS = 20;
constinit const int MAX_HEALTH = 100;
constinit const int MAX_UPPED_HEALTH = 200;
constinit const int MAX_ARMOR = 100;
constinit const int MAX_UPPED_ARMOR = 200;
constinit const float ITEM_RESPAWN_TIMEOUT = 15.f;
constinit const float ITEM_SPAWN_EFFECT_TIMEOUT = 0.5f;
constinit const unsigned SHOTGUN_PELLET_AMOUNT = 10;
constinit const float SHOTGUN_SPREAD = 0.1f;

constinit const int MAX_BULLETS = 99;
constinit const int MAX_SHELLS = 24;
constinit const int MAX_ENERGY = 18;
constinit const int MAX_ROCKETS = 18;

constinit const std::array<AmmoCounterType, 4> AMMO_LIMIT = {
    MAX_BULLETS, MAX_SHELLS, MAX_ENERGY, MAX_ROCKETS
};

// Colors
const sf::Color COLOR_PICO8_RED = sf::Color(255, 0, 77);
const sf::Color COLOR_PICO8_GREEN = sf::Color(0, 228, 54);
const sf::Color COLOR_PICO8_MAGENTA = sf::Color(255, 119, 168);

// Ai
constinit const float SEEK_TIMEOUT = 0.2f;
constinit const float AI_MAX_AIM_ERROR = std::numbers::pi_v<float> / 16;
constinit const float AI_MAX_POSITION_ERROR = 2.f;
constinit const float AI_TURN_SPEED_MULTIPLIER = 2.f;
constinit const float AI_FOV = 90.f;
constinit const float AI_REACTION_TIME = 0.5f;
constinit const float AI_FLASH_REACTION_TIME = 0.1f;

// Inventory
constinit const unsigned WEAPON_COUNT = 16;

[[nodiscard]] constexpr static WeaponIndexType
weaponTypeToIndex(EntityType type) noexcept
{
    return static_cast<WeaponIndexType>(type)
           - static_cast<WeaponIndexType>(EntityType::WeaponFlaregun);
}

[[nodiscard]] constexpr static WeaponIndexType
weaponPickupToIndex(EntityType type) noexcept
{
    return static_cast<WeaponIndexType>(type)
           - static_cast<WeaponIndexType>(EntityType::WeaponPickableBegin);
}

[[nodiscard]] constexpr static EntityType
weaponIndexToType(WeaponIndexType idx) noexcept
{
    return static_cast<EntityType>(
        static_cast<WeaponIndexType>(EntityType::WeaponFlaregun) + idx);
}

[[nodiscard]] constexpr static std::size_t
ammoPickupToAmmoIndex(EntityType type) noexcept
{
    return static_cast<std::size_t>(type)
           - static_cast<std::size_t>(EntityType::PickupBullets);
}

[[nodiscard]] constexpr static std::size_t
ammoTypeToAmmoIndex(AmmoType type) noexcept
{
    return static_cast<std::size_t>(type);
}

[[nodiscard]] constexpr static EntityType
ammoTypeToPickupType(AmmoType type) noexcept
{
    return static_cast<EntityType>(
        static_cast<std::size_t>(type)
        + static_cast<std::size_t>(EntityType::PickupBullets));
}

// Other
constinit const std::size_t ANIMATION_FPS = 60 / 4; // four updates per second
constinit const unsigned MAX_LEVEL_WIDTH = 32;
constinit const unsigned MAX_LEVEL_HEIGHT = 32;
const sf::Vector2f NULL_VECTOR = { 0.f, 0.f };
constinit const float EPSILON = 0.001f;
constinit const float OVERLAY_INTENSITY_DECAY_RATE = 96_unitspersec;
constinit const float HUD_MESSAGE_DURATION = 2_seconds;
// #define DEBUG_REMOVALS 0
