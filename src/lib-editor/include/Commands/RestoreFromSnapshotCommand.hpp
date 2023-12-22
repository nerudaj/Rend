#pragma once

#include "Interfaces/CommandInterface.hpp"
#include "Interfaces/EditorInterface.hpp"
#include <LevelD.hpp>

class RestoreFromSnapshotCommand final : public CommandInterface
{
public:
    [[nodiscard]] RestoreFromSnapshotCommand(
        EditorInterface& editor, LevelD snapshot) noexcept
        : editor(editor), snapshot(snapshot)
    {
    }

public:
    void exec() override
    {
        /* FIXME:
        editor.loadFrom(snapshot, snapshot.metadata.description, true);*/
    }

protected:
    EditorInterface& editor;
    LevelD snapshot;
};
