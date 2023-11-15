#include "Commands/ResizeCommand.hpp"
#include "Commands/RestoreFromSnapshotCommand.hpp"

void ResizeCommand::exec()
{
    levelSnapshot = editor.save();
    editor.resize(width, height, isTranslationDisabled);
}

mem::Box<CommandInterface> ResizeCommand::getInverse() const
{
    return mem::Box<RestoreFromSnapshotCommand>(editor, levelSnapshot);
}
