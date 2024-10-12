#pragma once

#include <cstdint>

enum class [[nodiscard]] ClientMessageType : uint8_t
{
    PeerSettingsUpdate,
    LobbySettingsUpdate,
    ReportPeerReady,
    ReportMapReady,
    ReportInput,
    ReportMapEnded,
    Disconnect,
    MapDownloadRequest,
    Heartbeat,
};
