#include "Commands/SetTilePropertyCommand.hpp"

void SetTilePropertyCommand::exec()
{
    oldTileValue = map.getTileValue(tileCoord);
    oldSolid = map.isTileSolid(tileCoord);

    map.setTileValue(tileCoord, tileValue);
    map.setTileSolid(tileCoord, solid);
}

mem::Box<CommandInterface> SetTilePropertyCommand::getInverse() const
{
    return mem::Box<SetTilePropertyCommand>(
        map, tileCoord, *oldTileValue, *oldSolid);
}
