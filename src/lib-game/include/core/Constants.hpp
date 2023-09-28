#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <core/Enums.hpp>
#include <map>

// Movement
const float PLAYER_RADIAL_SPEED = 88.f;  // degrees per second
const float PLAYER_FORWARD_SPEED = 96.f; // pixels per second
const float PLAYER_STRAFE_SPEED = 96.f;

// Physics
const float PLAYER_RADIUS = 4.f;
const float PX2_RADIUS = 2.f;
const float PX3_RADIUS = 3.f;
const float PX4_RADIUS = 4.f;
const float PX6_RADIUS = 6.f;
const float EXPLOSION_RADIUS = 8.f;

const static inline std::map<LeveldItemId, float> THING_RADIUSES = {
    { LeveldItemId::Medikit, PX3_RADIUS },
    { LeveldItemId::ArmorShard, PX3_RADIUS },
    { LeveldItemId::MegaHealth, PX6_RADIUS },
    { LeveldItemId::MegaArmor, PX3_RADIUS },
    { LeveldItemId::Bullets, PX4_RADIUS },
    { LeveldItemId::Shells, PX3_RADIUS },
    { LeveldItemId::EnergyPack, PX4_RADIUS },
    { LeveldItemId::Rockets, PX3_RADIUS },
    { LeveldItemId::Shotgun, PX6_RADIUS },
    { LeveldItemId::Pillar, PX3_RADIUS },
    { LeveldItemId::FloorLamp, PX3_RADIUS }
};

// Game logic
const int PLAYER_INITIAL_HEALTH = 100;
const int PLAYER_INITIAL_ARMOR = 0;
const int PLAYER_INITIAL_BULLETS = 50;
const int PLAYER_INITIAL_NONBULLET_AMMO = 0;
const int MEDIKIT_HEALTH_AMOUNT = 25;
const int ARMORSHARD_ARMOR_AMOUNT = 10;
const int MEGAHEALTH_HEALTH_AMOUNT = 100;
const int MEGAARMOR_ARMOR_AMOUNT = 100;
const int BULLET_AMOUNT = 100;
const int SHELL_AMOUNT = 20;
const int ENERGY_AMOUNT = 20;
const int ROCKET_AMOUNT = 10;
const int MAX_BASE_HEALTH = 100;
const int MAX_UPPED_HEALTH = 200;
const int MAX_ARMOR = 200;
const int MAX_BULLETS = 200;
const int MAX_SHELLS = 50;
const int MAX_ENERGY = 100;
const int MAX_ROCKETS = 25;
const float WEAPON_LAUNCHER_COOLDOWN = 1.f;
const float PROJECTILE_FORWARD_SPEED = 108.f;
const int ROCKET_DAMAGE = 150;

// Other
const std::size_t ANIMATION_FPS = 60 / 4; // four updates per second
const unsigned MAX_LEVEL_WIDTH = 32;
const unsigned MAX_LEVEL_HEIGHT = 32;
const unsigned MAX_PLAYER_COUNT = 4;
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
