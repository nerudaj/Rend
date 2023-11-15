#include "include/Commands/SetTilePropertyCommand.hpp"

void SetTilePropertyCommand::exec()
{
	oldTileValue = map.getTileValue(tileCoord);
	oldSolid = map.isTileSolid(tileCoord);

	map.setTileValue(tileCoord, tileValue);
	map.setTileSolid(tileCoord, solid);
}

Box<CommandInterface> SetTilePropertyCommand::getInverse() const
{
	return Box<SetTilePropertyCommand>(
		map,
		tileCoord,
		*oldTileValue,
		*oldSolid);
}
