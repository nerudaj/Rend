#pragma once

#include <cstdint>

enum class [[nodiscard]] ServerMessageType : uint8_t
{
    ConnectionAccepted,
    ConnectionRefused,
    Update
};
