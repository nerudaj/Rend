#include "LevelMesh/PollableTileMap.hpp"
#include "Utilities/Utilities.hpp"
#include <cassert>

unsigned PollableTileMap::getTileValue(unsigned tileX, unsigned tileY) const
{
    unsigned tileI = tileY * dataSize.x + tileX;
    const sf::Vertex* quad = &vertices[tileI * size_t(4)];

    for (unsigned i = 0; i < clip.getFrameCount(); i++)
    {
        if (quad[0].texCoords.x == clip.getFrame(i).left
            && quad[0].texCoords.y == clip.getFrame(i).top)
            return i;
    }

    assert(false);
    return 0;
}
