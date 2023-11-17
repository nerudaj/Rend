#include "Commands/SetTileAreaCommand.hpp"
#include <algorithm>

void SetTileAreaCommand::exec()
{
    using TileEdit = SetTileAreaInverseCommand::TileEdit;

    const auto width = targetMesh.getMapDimensions().x;
    const auto height = targetMesh.getMapDimensions().y;

    const auto aX = std::clamp(start.x, 0u, width - 1);
    const auto aY = std::clamp(start.y, 0u, height - 1);
    const auto bX = std::clamp(end.x, 0u, width - 1);
    const auto bY = std::clamp(end.y, 0u, height - 1);

    const unsigned sX = std::min(aX, bX);
    const unsigned sY = std::min(aY, bY);
    const unsigned eX = std::max(aX, bX);
    const unsigned eY = std::max(aY, bY);

    for (unsigned y = sY; y <= eY; y++)
    {
        for (unsigned x = sX; x <= eX; x++)
        {
            if (!(fill || (x == sX || x == eX || y == sY || y == eY))) continue;

            reverseEdits.push_back(TileEdit {
                .pos = { x, y },
                .value = targetMesh.getTileValue({ x, y }),
                .blocking = bool(targetMesh.isTileSolid({ x, y })) });

            targetMesh.setTileValue({ x, y }, value);
            targetMesh.setTileSolid({ x, y }, blocking);
        }
    }
}

mem::Box<CommandInterface> SetTileAreaCommand::getInverse() const
{
    return mem::Box<SetTileAreaInverseCommand>(targetMesh, reverseEdits);
}

void SetTileAreaInverseCommand::exec()
{
    for (auto&& edit : tilesToEdit)
    {
        targetMesh.setTileValue(edit.pos, edit.value);
        targetMesh.setTileSolid(edit.pos, edit.blocking);
    }
}
