#include "include/Commands/ResizeCommand.hpp"
#include "include/Commands/RestoreFromSnapshotCommand.hpp"

void ResizeCommand::exec()
{
    levelSnapshot = editor.save();
    editor.resize(width, height, isTranslationDisabled);
}

Box<CommandInterface> ResizeCommand::getInverse() const
{
    return Box<RestoreFromSnapshotCommand>(editor, levelSnapshot);
}
