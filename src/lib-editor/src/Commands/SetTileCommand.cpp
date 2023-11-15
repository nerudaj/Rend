#include "include/Commands/SetTileCommand.hpp"

void SetTileCommand::exec()
{
	oldValue = targetMesh.getTileValue(tilePos);
	oldBlocking = targetMesh.isTileSolid(tilePos);

	targetMesh.setTileValue(tilePos, value);
	targetMesh.setTileSolid(tilePos, blocking);
}

Box<CommandInterface> SetTileCommand::getInverse() const
{
	return Box<SetTileCommand>(
		targetMesh,
		tilePos,
		*oldValue,
		*oldBlocking);
}
