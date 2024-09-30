#pragma once

#include <cstdint>

enum class [[nodiscard]] SpawnRotation : uint16_t
{
    Right = 0,
    DownRight,
    Down,
    DownLeft,
    Left,
    UpLeft,
    Up,
    UpRight
};
