#include "include/Commands/ShrinkToFitCommand.hpp"
#include "include/Commands/RestoreFromSnapshotCommand.hpp"

void ShrinkToFitCommand::exec()
{
	levelSnapshot = editor.save();
	editor.shrinkToFit();
}

Box<CommandInterface> ShrinkToFitCommand::getInverse() const
{
	return Box<RestoreFromSnapshotCommand>(
		editor,
		levelSnapshot);
}
