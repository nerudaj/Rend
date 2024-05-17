#pragma once

#include <cstdint>

enum class [[nodiscard]] ClientMessageType : uint8_t
{
    ConnectionRequest,
    PeerSettingsUpdate,
    LobbySettingsUpdate,
    ReportPeerReady,
    ReportMapReady,
    ReportInput,
    ReportMapEnded,
    Disconnect
};
