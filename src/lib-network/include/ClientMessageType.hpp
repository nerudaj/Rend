#pragma once

#include <cstdint>

enum class [[nodiscard]] ClientMessageType : uint8_t {
    ConnectionRequest,
    PeerSettingsUpdate,
    GameSettingsUpdate,
    CommitLobby,
    MapLoaded,
    ReportInput,
    MapEnded,
    Disconnect
};
