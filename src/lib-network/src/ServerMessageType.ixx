module;

#include <cstdint>

export module ServerMessageType;

export enum class [[nodiscard]] ServerMessageType : uint8_t {
    ConnectionAccepted,
    ConnectionRefused,
    Update
};
