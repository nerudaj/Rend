#include "Commands/ShrinkToFitCommand.hpp"
#include "Commands/RestoreFromSnapshotCommand.hpp"

void ShrinkToFitCommand::exec()
{
    levelSnapshot = editor.save();
    editor.shrinkToFit();
}

mem::Box<CommandInterface> ShrinkToFitCommand::getInverse() const
{
    return mem::Box<RestoreFromSnapshotCommand>(editor, levelSnapshot);
}
