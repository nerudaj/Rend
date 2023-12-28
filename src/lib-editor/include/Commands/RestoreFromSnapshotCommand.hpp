#pragma once

#include "Interfaces/CommandInterface.hpp"
#include "Interfaces/EditorInterface.hpp"
#include <LevelD.hpp>

class [[nodiscard]] RestoreFromSnapshotCommand final : public CommandInterface
{
public:
    RestoreFromSnapshotCommand(
        EditorInterface& editor, LevelD snapshot) noexcept
        : editor(editor), snapshot(snapshot)
    {
    }

public:
    void exec() override
    {
        editor.restoreFromSnapshot(snapshot);
    }

protected:
    EditorInterface& editor;
    LevelD snapshot;
};
