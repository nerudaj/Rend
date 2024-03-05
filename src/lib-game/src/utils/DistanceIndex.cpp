#include "utils/DistanceIndex.hpp"
#include <bitset>
#include <print>

DistanceIndex::DistanceIndex(const dgm::Mesh& mesh)
    : tilesPerRow(mesh.getDataSize().x)
    , voxelSize(static_cast<float>(mesh.getVoxelSize().x))
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

    try
    {
        return index
            .at(std::min(fromTile, toTile, dgm::Utility::less<sf::Vector2u> {}))
            .at(std::max(
                fromTile, toTile, dgm::Utility::less<sf::Vector2u> {}));
    }
    catch (...)
    {
        return 1000;
    }
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

DistanceIndex2::DistanceIndex2(const dgm::Mesh& mesh)
    : tilesPerRow(mesh.getDataSize().x)
    , voxelSize(static_cast<float>(mesh.getVoxelSize().x))
{
    const auto limit = mesh.getDataSize().x * mesh.getDataSize().y;
    const auto&& start = mesh.getDataSize().x + 2;
    const auto&& end = limit - mesh.getDataSize().x - 1;
    for (unsigned i = start; i < end; i++)
    {
        if (mesh[i] == 1) continue;

        unsigned j = i + 1;
        for (j; j < end; j++)
        {
            if (mesh[j] == 1) break;
            const auto from = toTileCoord(i);
            const auto to = toTileCoord(j);
            index[from][to] = j - i;
        }

        for (j; j < end; j++)
        {
            if (mesh[j] == 1)
            {
                continue;
            }

            const auto from = toTileCoord(i);
            const auto to = toTileCoord(j);
            const auto path = dgm::TileNavMesh::computePath(from, to, mesh);
            if (!path) continue;

            index[from][to] = static_cast<int>(path.value().getLength());
        }
    }
}

DistanceIndex3::DistanceIndex3(const dgm::Mesh& mesh)
    : tilesPerRow(mesh.getDataSize().x)
    , voxelSize(static_cast<float>(mesh.getVoxelSize().x))
{

    const auto limit = mesh.getDataSize().x * mesh.getDataSize().y;
    const auto&& start = mesh.getDataSize().x + 1;
    const auto&& end = limit - start;
    auto&& manhattan = std::vector<int>(limit, -1);
    for (unsigned i = start, group = 1; i < end; ++i)
    {
        const auto xLess = manhattan[i - 1];
        const auto yLess = manhattan[i - mesh.getDataSize().x];
        if (mesh[i] == 1) continue;

        if (xLess == -1 && yLess == -1)
            manhattan[i] = group++;
        else
            manhattan[i] = xLess == -1   ? yLess
                           : yLess == -1 ? xLess
                                         : std::min(xLess, yLess);
    }

    index = decltype(index)(end - start);

    unsigned countM = 0, countD = 0;
    for (unsigned i = start; i < end; i++)
    {
        index[i - start] = std::vector<int>(end - i, 1000u);

        if (mesh[i] == 1) continue;

        for (unsigned j = i + 1; j < end; j++)
        {
            if (mesh[j] == 1) continue;

            const auto from = toTileCoord(i);
            const auto to = toTileCoord(j);
            if (manhattan[i] == manhattan[j])
            {
                index[i - start][j - i] = to.x - from.x + to.y - from.y;
                ++countM;
            }
            else
            {
                const auto path = dgm::TileNavMesh::computePath(from, to, mesh);
                if (!path) continue;

                index[i - start][j - i] =
                    static_cast<int>(path.value().getLength());
                ++countD;
            }
        }
    }

    std::println("{}/{}", countM, countD);
}

DistanceIndex4::DistanceIndex4(const dgm::Mesh& mesh)
    : tilesPerRow(mesh.getDataSize().x)
    , voxelSize(static_cast<float>(mesh.getVoxelSize().x))
{
    // TODO: Layered manhattans

    const auto limit = mesh.getDataSize().x * mesh.getDataSize().y;
    const auto&& start = mesh.getDataSize().x + 1;
    const auto&& end = limit - start;

    index = decltype(index)(end - start);

    std::bitset<1024> manhattan;

    for (unsigned i = start; i < end; i++)
    {
        unsigned countM = 0, countD = 0;
        if (mesh[i] == 1) continue;
        index[i - start] = std::vector<int>(end - i, 0);
        manhattan.reset();
        manhattan[i] = true;

        for (unsigned j = i + 1; j < end; j++)
        {
            if (mesh[j] == 1) continue;

            manhattan[j] =
                manhattan[j - 1] || manhattan[j - mesh.getDataSize().x];

            const auto from = toTileCoord(i);
            const auto to = toTileCoord(j);
            if (manhattan[j])
            {
                // TODO: use appropriate manhattan
                index[i - start][j - i] = to.x - from.x + to.y - from.y;
                ++countM;
            }
            else
            {
                const auto path = dgm::TileNavMesh::computePath(from, to, mesh);
                if (!path) continue;

                index[i - start][j - i] =
                    static_cast<int>(path.value().getLength());
                ++countD;
            }
        }
        std::println(" {}/{}", countM, countD);
    }
}

void computeDistances(
    const dgm::Mesh& mesh,
    std::vector<unsigned>& lookup,
    unsigned origX,
    unsigned origY)
{
    struct Point
    {
        unsigned position;
        unsigned value;
    };

    auto toIndex = [&mesh](unsigned x, unsigned y)
    { return y * mesh.getDataSize().x + x; };
    const auto&& startIndex = toIndex(origX, origY);

    std::deque<Point> deque = { Point { .position = startIndex, .value = 0 } };

    while (!deque.empty())
    {
        auto item = deque.front();
        deque.pop_front();

        if (mesh[item.position] == 1 || lookup[item.position] <= item.value)
            continue;

        lookup[item.position] = item.value;

        deque.emplace_back(item.position - 1, item.value + 1);
        deque.emplace_back(item.position + 1, item.value + 1);
        deque.emplace_back(
            item.position - mesh.getDataSize().x, item.value + 1);
        deque.emplace_back(
            item.position - mesh.getDataSize().x, item.value + 1);
    }
}

DistanceIndex5::DistanceIndex5(const dgm::Mesh& mesh)
    : tilesPerRow(mesh.getDataSize().x)
    , voxelSize(static_cast<float>(mesh.getVoxelSize().x))
{
    const auto limit = mesh.getDataSize().x * mesh.getDataSize().y;
    const auto&& start = mesh.getDataSize().x + 1;
    const auto&& end = limit - start;

    index = decltype(index)(end - start);

    for (unsigned y = 1; y < mesh.getDataSize().y - 1; ++y)
    {
        for (unsigned x = 1; x < mesh.getDataSize().x - 1; ++x)
        {
            unsigned i = y * mesh.getDataSize().x + x;
            if (mesh[i] == 1) continue;

            index[i - start] = std::vector<unsigned>(limit, 2000u);
            computeDistances(mesh, index[i - start], x, y);
        }
    }
}
