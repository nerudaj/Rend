#include "utils/DistanceIndex.hpp"

DistanceIndex::DistanceIndex(const dgm::Mesh& mesh)
    : tilesPerRow(mesh.getDataSize().x), voxelSize(mesh.getVoxelSize().x)
{
    const auto limit = mesh.getDataSize().x * mesh.getDataSize().y;
    for (unsigned i = 0; i < limit; i++)
    {
        if (mesh[i] == 1) continue;

        for (unsigned j = i + 1; j < limit; j++)
        {
            if (mesh[j] == 1) continue;

            const auto from = toTileCoord(i);
            const auto to = toTileCoord(j);
            const auto path = dgm::TileNavMesh::computePath(from, to, mesh);
            if (!path) continue;

            index[from][to] = static_cast<int>(path.value().getLength());
        }
    }
}

int DistanceIndex::getDistance(
    const sf::Vector2f& from, const sf::Vector2f& to) const
{
    const auto fromTile = toTileCoord(from);
    const auto toTile = toTileCoord(to);

    if (fromTile == toTile) return 0;

    return index
        .at(std::min(fromTile, toTile, dgm::Utility::less<sf::Vector2u> {}))
        .at(std::max(fromTile, toTile, dgm::Utility::less<sf::Vector2u> {}));
}

sf::Vector2u DistanceIndex::toTileCoord(unsigned meshIndex) const noexcept
{
    return sf::Vector2u(meshIndex % tilesPerRow, meshIndex / tilesPerRow);
}

sf::Vector2u
DistanceIndex::toTileCoord(const sf::Vector2f& worldCoord) const noexcept
{
    return sf::Vector2u(worldCoord / voxelSize);
}
