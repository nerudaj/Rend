module;

#include <cstdint>

export module ClientMessageType;

export enum class [[nodiscard]] ClientMessageType : uint8_t {
    ConnectionRequest,
    PeerSettingsUpdate,
    GameSettingsUpdate,
    CommitLobby,
    MapLoaded,
    ReportInput,
    Disconnect
};
