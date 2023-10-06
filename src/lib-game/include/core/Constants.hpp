#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <core/Enums.hpp>
#include <core/Types.hpp>
#include <map>
#include <utils/SemanticTypes.hpp>

// Movement
constinit const float PLAYER_RADIAL_SPEED = 88.f;  // degrees per second
constinit const float PLAYER_FORWARD_SPEED = 96.f; // pixels per second
constinit const float PLAYER_STRAFE_SPEED = 96.f;
constinit const float PROJECTILE_FORWARD_SPEED = 108.f;

// Game logic
constinit const unsigned FPS = 60;
constinit const float FRAME_TIME = 1.f / FPS;
constinit const int PLAYER_INITIAL_HEALTH = 100;
constinit const int PLAYER_INITIAL_ARMOR = 0;
constinit const int PLAYER_INITIAL_BULLETS = 50;
constinit const int PLAYER_INITIAL_NONBULLET_AMMO = 0;
constinit const int MEDIKIT_HEALTH_AMOUNT = 25;
constinit const int ARMORSHARD_ARMOR_AMOUNT = 10;
constinit const int MEGAHEALTH_HEALTH_AMOUNT = 100;
constinit const int MEGAARMOR_ARMOR_AMOUNT = 100;
constinit const int BULLET_AMOUNT = 100;
constinit const int SHELL_AMOUNT = 20;
constinit const int ENERGY_AMOUNT = 20;
constinit const int ROCKET_AMOUNT = 10;
constinit const int MAX_BASE_HEALTH = 100;
constinit const int MAX_UPPED_HEALTH = 200;
constinit const int MAX_ARMOR = 200;
constinit const int MAX_BULLETS = 200;
constinit const int MAX_SHELLS = 50;
constinit const int MAX_ENERGY = 100;
constinit const int MAX_ROCKETS = 25;
constinit const int ROCKET_DAMAGE = 150;
constinit const float ITEM_RESPAWN_TIMEOUT = 15.f;
constinit const float ITEM_SPAWN_EFFECT_TIMEOUT = 0.5f;

// Inventory
constinit const unsigned WEAPON_COUNT = 16;

[[nodiscard]] constexpr static WeaponIndexType
weaponTypeToIndex(EntityType type) noexcept
{
    return static_cast<WeaponIndexType>(type)
           - static_cast<WeaponIndexType>(EntityType::WeaponFlaregun);
}

[[nodiscard]] constexpr static EntityType
weaponIndexToType(WeaponIndexType idx) noexcept
{
    return static_cast<EntityType>(
        static_cast<WeaponIndexType>(EntityType::WeaponFlaregun) + idx);
}

// Other
constinit const std::size_t ANIMATION_FPS = 60 / 4; // four updates per second
constinit const unsigned MAX_LEVEL_WIDTH = 32;
constinit const unsigned MAX_LEVEL_HEIGHT = 32;
constinit const unsigned MAX_PLAYER_COUNT = 4;
const sf::Vector2f NULL_VECTOR = { 0.f, 0.f };
constinit const float EPSILON = 0.001f;
