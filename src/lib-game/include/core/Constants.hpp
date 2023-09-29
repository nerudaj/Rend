#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <core/Enums.hpp>
#include <map>
#include <utils/SemanticTypes.hpp>

// Movement
constinit const float PLAYER_RADIAL_SPEED = 88.f;  // degrees per second
constinit const float PLAYER_FORWARD_SPEED = 96.f; // pixels per second
constinit const float PLAYER_STRAFE_SPEED = 96.f;

// Physics
const float PLAYER_RADIUS = 4.f;

const static inline std::map<LeveldItemId, float> THING_RADIUSES = {
    { LeveldItemId::Medikit, 3_px },    { LeveldItemId::ArmorShard, 3_px },
    { LeveldItemId::MegaHealth, 6_px }, { LeveldItemId::MegaArmor, 3_px },
    { LeveldItemId::Bullets, 4_px },    { LeveldItemId::Shells, 3_px },
    { LeveldItemId::EnergyPack, 4_px }, { LeveldItemId::Rockets, 3_px },
    { LeveldItemId::Shotgun, 6_px },    { LeveldItemId::Pillar, 3_px },
    { LeveldItemId::FloorLamp, 3_px }
};

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
constinit const float WEAPON_LAUNCHER_COOLDOWN = 1.f;
constinit const float PROJECTILE_FORWARD_SPEED = 108.f;
constinit const int ROCKET_DAMAGE = 150;

// Other
constinit const std::size_t ANIMATION_FPS = 60 / 4; // four updates per second
constinit const unsigned MAX_LEVEL_WIDTH = 32;
constinit const unsigned MAX_LEVEL_HEIGHT = 32;
constinit const unsigned MAX_PLAYER_COUNT = 4;
const sf::Vector2f NULL_VECTOR = { 0.f, 0.f };

[[nodiscard]] static constexpr bool isDirectional(EntityType type) noexcept
{
    using enum EntityType;
    return type == Player || type == ProjectileRocket;
}

[[nodiscard]] static constexpr bool isDrawable(EntityType type) noexcept
{
    return !(EntityType::MarkerBegin <= type && type <= EntityType::MarkerEnd);
}

[[nodiscard]] static constexpr bool isAnimable(EntityType type) noexcept
{
    using enum EntityType;
    return type == ProjectileRocket || type == EffectClip;
}

[[nodiscard]] static constexpr inline bool
isDestructible(EntityType type) noexcept
{
    return type == EntityType::Player;
}

[[nodiscard]] static constexpr inline bool isSolid(EntityType type) noexcept
{
    using enum EntityType;
    return type == StaticDecoration || type == Player;
}

[[nodiscard]] static constexpr inline bool isPickable(EntityType type) noexcept
{
    using enum EntityType;
    return PickableBegin < type && type < PickableEnd;
}

[[nodiscard]] static constexpr inline bool
isWeaponPickable(EntityType type) noexcept
{
    using enum EntityType;
    return WeaponPickableBegin < type && type < WeaponPickableEnd;
}
