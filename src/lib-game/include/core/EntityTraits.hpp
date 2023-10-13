#pragma once

#include <core/Enums.hpp>

[[nodiscard]] static constexpr bool isDirectional(EntityType type) noexcept
{
    using enum EntityType;
    return type == Player || type == ProjectileRocket
           || type == ProjectileLaserDart;
}

[[nodiscard]] static constexpr inline bool
isDestructible(EntityType type) noexcept
{
    return type == EntityType::Player;
}

[[nodiscard]] static constexpr inline bool isSolid(EntityType type) noexcept
{
    using enum EntityType;
    return (StaticDecorationBegin <= type && type <= StaticDecorationEnd)
           || type == Player;
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

[[nodiscard]] static constexpr inline bool
isPowerItemPickable(EntityType type) noexcept
{
    using enum EntityType;
    return PowerItemsBegin < type && type < PowerItemsEnd;
}

[[nodiscard]] static constexpr inline bool
isProjectile(EntityType type) noexcept
{
    using enum EntityType;
    return ProjectilesBegin < type && type < ProjectilesEnd;
}

[[nodiscard]] static constexpr inline bool
isBouncyProjectile(EntityType type) noexcept
{
    using enum EntityType;
    return type == ProjectileLaserDart;
}